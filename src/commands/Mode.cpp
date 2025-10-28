/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:37:30 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 17:43:01 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Mode.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <sstream>
#include <string>
#include <vector>

Mode::Mode(Server *server) : Command(server)
{
}

Mode::~Mode()
{
}

static bool	is_digits(const std::string &s)
{
	for (size_t i = 0; i < s.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return (false);
	return (!s.empty());
}

static std::string	currentModesString(Channel *ch)
{
	std::string	flags;
	std::string	params;
	if (ch->hasMode('i'))
		flags += 'i';
	if (ch->hasMode('t'))
		flags += 't';
	if (ch->hasMode('k'))
	{
		flags += 'k';
		params += " " + ch->getModeParam('k');
	}
	if (ch->hasMode('l'))
	{
		flags += 'l';
		params += " " + ch->getModeParam('l');
	}
	if (flags.empty())
		flags = "+";
	else
		flags = "+" + flags;
	return (flags + params);
}

void	Mode::execute(Client *client, const std::vector<std::string> &params)
{
	int	index = _server->getClientIndex(client);
	if (params.empty())
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "MODE",
					":Not enough parameters");
		return ;
	}
	const std::string	&target = params[0];
	Channel	*ch = _server->getChannel(target);
	if (!ch)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOSUCHCHANNEL, "MODE",
					target + " :No such channel");
		return ;
	}
	if (params.size() == 1)
	{
		std::string reply = ":ircserv " + intToString(RPL_CHANNELMODEIS) + " " + client->getNick() + " " + target +
			" " + currentModesString(ch) + "\r\n";
		_server->sendMessageToClient(client, reply);
		return ;
	}
	if (!ch->isClientInChannel(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOTONCHANNEL, "MODE",
					target + " :You're not on that channel");
		return ;
	}
	if (!ch->isOperator(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_CHANOPRIVSNEEDED, "MODE",
					target + " :You're not channel operator");
		return ;
	}
	const std::string	&modeStr = params[1];
	bool	adding = true;
	size_t	argIndex = 2;
	std::string	appliedFlags;
	std::string	currentModeString;
	bool	currentAdding = true;
	std::vector<std::string>	appliedParams;
	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char	c = modeStr[i];
		if (c == '+')
		{
			if (!currentModeString.empty())
			{
				appliedFlags += (currentAdding ? "+" : "-") + currentModeString;
				currentModeString.clear();
			}
			adding = true;
			currentAdding = true;
			continue ;
		}
		if (c == '-')
		{
			if (!currentModeString.empty())
			{
				appliedFlags += (currentAdding ? "+" : "-") + currentModeString;
				currentModeString.clear();
			}
			adding = false;
			currentAdding = false;
			continue ;
		}
		if (!ch->isValidMode(c))
		{
			if (index != -1)
				_server->sendErrorMessage(index, ERR_UNKNOWNMODE, "MODE",
						std::string(1, c) + " :is unknown mode char");
			continue ;
		}
		switch (c)
		{
			case 'i':
				if (adding)
					ch->setMode('i', "");
				else
					ch->unsetMode('i');
				currentModeString += c;
				break;
			case 't':
				if (adding)
					ch->setMode('t', "");
				else
					ch->unsetMode('t');
				currentModeString += c;
				break;
			case 'k':
				if (adding)
				{
					if (argIndex >= params.size())
					{
						if (index != -1)
							_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "MODE",
									" :Not enough parameters");
						break;
					}
					ch->setMode('k', params[argIndex]);
					currentModeString += c;
					appliedParams.push_back(params[argIndex]);
					argIndex++;
				}
				else
				{
					ch->unsetMode('k');
					currentModeString += c;
				}
				break;
			case 'l':
				if (adding)
				{
					if (argIndex >= params.size())
					{
						if (index != -1)
							_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "MODE",
									" :Not enough parameters");
						break;
					}
					if (!is_digits(params[argIndex]))
					{
						if (index != -1)
							_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "MODE",
									" :Invalid limit");
						break;
					}
					ch->setMode('l', params[argIndex]);
					currentModeString += c;
					appliedParams.push_back(params[argIndex]);
					argIndex++;
				}
				else
				{
					ch->unsetMode('l');
					currentModeString += c;
				}
				break;
			case 'o':
				{
					if (argIndex >= params.size())
					{
						if (index != -1)
							_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "MODE",
									" :Not enough parameters");
						break;
					}
					Client *targetUser = _server->getClientByNick(params[argIndex]);
					if (!targetUser)
					{
						if (index != -1)
							_server->sendErrorMessage(index, ERR_NOSUCHNICK, "MODE",
									params[argIndex] + " :No such nick");
						break;
					}
					if (!ch->isClientInChannel(targetUser))
					{
						if (index != -1)
							_server->sendErrorMessage(index, ERR_USERNOTINCHANNEL, "MODE",
								params[argIndex] + " " + target +
									" :They aren't on that channel");
						break;
					}
					if (adding)
					{
						ch->addOperator(targetUser);
						currentModeString += c;
						appliedParams.push_back(params[argIndex]);
					}
					else
					{
						ch->removeOperator(targetUser);
						currentModeString += c;
						appliedParams.push_back(params[argIndex]);
					}
					argIndex++;
					break;
				}
		}
    }
	if (!currentModeString.empty())
	{
		appliedFlags += (currentAdding ? "+" : "-") + currentModeString;
	}
	if (!appliedFlags.empty())
	{
		std::ostringstream oss;
		oss << ":" << client->_nickname << "!" << client->_username << "@"
			<< client->getHost() << " MODE " << target << " ";
		oss << appliedFlags;
		for (size_t i = 0; i < appliedParams.size(); ++i)
			oss << " " << appliedParams[i];
		oss << "\r\n";
		std::string mode_msg = oss.str();
		ch->broadcastMessage(mode_msg, client);
		_server->sendMessageToClient(client, mode_msg);
	}
}
