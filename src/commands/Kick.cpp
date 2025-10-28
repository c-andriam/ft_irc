/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:36:16 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 17:29:45 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Kick.hpp"
#include "../../include/Server.hpp"
#include "../../include/IrcReplies.h"
#include <cstddef>
#include <sstream>
#include <string>
#include <iostream>

Kick::Kick(Server* server) : Command(server)
{
}

Kick::~Kick()
{
}

void	Kick::execute(Client* client, const std::vector<std::string>& params)
{
	int	index = _server->getClientIndex(client);
	if (params.size() < 2)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "KICK",
					" :Not enough parameters");
		return ;
	}
	const std::string	&target_channel = params[0];
	if (target_channel.empty() || (target_channel[0] != '#' && target_channel[0] != '&'))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_BADCHANMASK, "KICK",
					" :Bad Channel Mask");
		return ;
	}
	Channel	*ch = _server->getChannel(target_channel);
	if (!ch)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOSUCHCHANNEL, "KICK",
					target_channel + " :No such channel");
		return ;
	}
	if (!ch->isClientInChannel(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOTONCHANNEL, "KICK",
					target_channel + " :You're not on that channel");
		return ;
	}
	if (!ch->isOperator(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_CHANOPRIVSNEEDED, "KICK",
					target_channel + " :You're not channel operator");
		return ;
	}
	Client *target_user = _server->getClientByNick(params[1]);
	if (!target_user || !ch->isClientInChannel(target_user))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_USERNOTINCHANNEL, "KICK",
				params[1] + " " + target_channel + " :They aren't on that channel");
		return ;
	}
	std::string	comment;
	if (params.size() >= 3)
	{
		std::string	first_param = params[2];
		if (!first_param.empty() && first_param[0] == ':')
		{
			first_param = first_param.substr(1);
		}
		std::ostringstream	oss;
		oss << first_param;
		for (size_t i = 3; i < params.size(); ++i)
		{
			oss << " " << params[i];
		}
		comment = oss.str();
	}
	else
	{
		comment = client->_nickname;
	}
	std::ostringstream oss;
	oss << ":" << client->_nickname << "!" << client->_username << "@"
			<< client->getHost() << " KICK " << target_channel << " " << params[1] << " :" << comment << "\r\n";
	std::string kick_msg = oss.str();
	//_server->sendMessageToClient(client, kick_msg);
	ch->broadcastMessage(kick_msg, NULL);
	ch->removeClient(target_user);
	//_server->sendMessageToClient(target_user, kick_msg);
}
