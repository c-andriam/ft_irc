/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:41:47 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:24 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Privmsg.hpp"
#include "../../include/Channel.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <string>

Privmsg::Privmsg(Server *server) : Command(server)
{
}

Privmsg::~Privmsg()
{
}

void	Privmsg::execute(Client *client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		int index = _server->getClientIndex(client);
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NORECIPIENT, "",
					" :No recipient given (PRIVMSG)");
		return ;
	}
	if (params.size() < 2)
	{
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOTEXTTOSEND, "",
					" :No text to send");
		return ;
	}
	std::string	target = params[0];
	std::string	msg = params[1];
	if (target.empty() || msg.empty())
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_NEEDMOREPARAMS, "PRIVMSG",
					" :Not enough parameters");
		return ;
	}
	std::string	privmsg = ":" + client->_nickname + "!" + client->_username +
		"@" + client->getHost() + " PRIVMSG " + target + " " +
		msg + "\r\n";
	if (target[0] == '#')
	{
		Channel	*channel = _server->getChannel(target);
		if (channel && channel->isClientInChannel(client))
			channel->broadcastMessage(privmsg, client);
		else
		{
			int	i = _server->getClientIndex(client);
			if (i != -1)
				_server->sendErrorMessage(i, ERR_CANNOTSENDTOCHAN, "PRIVMSG",
						target + " :Cannot send to channel");
		}
	}
	else
	{
		Client	*targetClient = _server->getClientByNick(target);
		if (targetClient)
			_server->sendMessageToClient(targetClient, privmsg);
		else
		{
			int	i = _server->getClientIndex(client);
			if (i != -1)
				_server->sendErrorMessage(i, ERR_NOSUCHNICK, "PRIVMSG",
						target + " :No such nick/channel");
		}
	}
}
