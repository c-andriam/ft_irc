/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:35:46 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 17:51:58 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Join.hpp"
#include "../../include/Channel.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <iostream>
#include <limits>
#include <string>
#include <vector>

Join::Join(Server *server) : Command(server)
{
}

Join::~Join()
{
}

void	Join::execute(Client *client, const std::vector<std::string> &params)
{
	if (!client->_registered)
	{
		int	index = _server->getClientIndex(client);
		if (index != -1)
			return ;
	}
	if (params.empty())
	{
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "JOIN",
					" :Not enough parameters");
		return ;
	}
	std::string	channel_name = params[0];
	std::string	key;
	if (params.size() > 1)
	{
		key = params[1];
	}
	else
		key = "";
	if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_NOSUCHCHANNEL, "JOIN",
					" :No such channel");
		return ;
	}
	Channel	*channel = _server->createChannel(channel_name);
	if (!channel)
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_NOSUCHCHANNEL, "JOIN",
					channel_name + " :No such channel");
		return ;
	}
	if (channel->hasClient(client))
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_USERONCHANNEL, "JOIN",
					channel_name + " :is already on channel");
		return ;
	}
	if (channel->isInviteOnly() && !channel->isInvited(client))
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_INVITEONLYCHAN, "JOIN",
					channel_name + " :Cannot join channel (+i)");
		return ;
	}
	if (channel->hasKey() && channel->getKey() != key)
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
		{
			_server->sendErrorMessage(i, ERR_BADCHANNELKEY, "JOIN",
					channel_name + " :Cannot join channel (+k)");
			return ;
		}
	}
	if (channel->hasUserLimit() && channel->isFull())
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_CHANNELISFULL, "JOIN",
					channel_name + " :Cannot join channel (+l)");
		return ;
	}
	if (!channel->addClient(client))
	{
		int	i = _server->getClientIndex(client);
		if (i != -1)
			_server->sendErrorMessage(i, ERR_NOSUCHCHANNEL, "JOIN",
					channel_name + " :No such channel");
		return ;
	}
	std::string	join_msg = ":" + client->_nickname + "!" + client->_username +
		"@" + client->getHost() + " JOIN " + channel_name + "\r\n";
	channel->broadcastMessage(join_msg, client);
	_server->sendMessageToClient(client, join_msg);
	std::string	topic = channel->getTopic();
	if (!topic.empty())
	{
		std::string	topic_msg = ":ircserv " + intToString(RPL_TOPIC) + " " +
			client->_nickname + " " + channel_name + " :" + topic + "\r\n";
		_server->sendMessageToClient(client, topic_msg);
	}
	else
	{
		std::string no_topic_msg = ":ircserv " + intToString(RPL_NOTOPIC) + " " +
			client->_nickname + " " + channel_name + " :No topic is set\r\n";
		_server->sendMessageToClient(client, no_topic_msg);
	}
	//std::vector<Client *>	members = channel->getClients();
	std::string	list_names = channel->getUserList();
	/*for (size_t i = 0; i < members.size(); ++i)
	{
		if (i == 0)
			channel->addOperator(members[i]);
		if (channel->isOperator(members[i]))
			list_names += "@";
		list_names += members[i]->_nickname;
		if (i < members.size() - 1)
			list_names += " ";
	}*/
	std::string	name_msg = ":ircserv " + intToString(RPL_NAMREPLY) + " " +
		client->_nickname + " = " + channel_name + " :" + list_names + "\r\n";
	_server->sendMessageToClient(client, name_msg);
	std::string	end_msg = ":ircserv " + intToString(RPL_ENDOFNAMES) + " " +
		client->_nickname + " " + channel_name + " :End of /NAMES list\r\n";
	_server->sendMessageToClient(client, end_msg);
}
