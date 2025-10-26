/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:34:40 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:58 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Invite.hpp"
#include "../../include/Channel.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <string>

Invite::Invite(Server *server) : Command(server)
{
}

Invite::~Invite()
{
}

void	Invite::execute(Client *client, const std::vector<std::string> &params)
{
	int	index = _server->getClientIndex(client);
	if (params.size() < 2)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "INVITE",
					" :Not enough parameters");
		return ;
	}
	const std::string	&targetNick = params[0];
	const std::string	&channelName = params[1];

	Channel	*ch = _server->getChannel(channelName);
	if (!ch)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOSUCHCHANNEL, "INVITE",
					channelName + " :No such channel");
		return ;
	}
	if (!ch->isClientInChannel(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOTONCHANNEL, "INVITE",
					channelName + " :You're not on that channel");
		return ;
	}
	if (ch->hasMode('i') && !ch->isOperator(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_CHANOPRIVSNEEDED, "INVITE",
					channelName + " :You're not channel operator");
		return ;
	}
	Client *target = _server->getClientByNick(targetNick);
	if (!target)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOSUCHNICK, "INVITE",
					targetNick + " :No such nick");
		return ;
	}
	if (ch->isClientInChannel(target))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_USERONCHANNEL, "INVITE",
					targetNick + " " + channelName + " :is already on channel");
		return ;
	}
	std::string rpl_inviting = ":ircserv " + intToString(RPL_INVITING) + " " +
		client->getNick() + " " + targetNick + " " + channelName + "\r\n";
	_server->sendMessageToClient(client, rpl_inviting);
	ch->addInvite(target);
	std::string invite_msg = ":" + client->_nickname + "!" + client->_username +
		"@" + client->getHost() + " INVITE " + targetNick + " :" + channelName + "\r\n";
	_server->sendMessageToClient(target, invite_msg);
}
