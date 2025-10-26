/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:42:51 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:27 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Topic.hpp"
#include "../../include/Channel.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <string>

Topic::Topic(Server *server) : Command(server)
{
}

Topic::~Topic()
{
}

void	Topic::execute(Client *client, const std::vector<std::string> &params)
{
	int	index = _server->getClientIndex(client);
	if (params.empty())
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "TOPIC",
					" :Not enough parameters");
		return ;
	}
	const std::string	&channel_name = params[0];
	Channel	*ch = _server->getChannel(channel_name);
	if (!ch)
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOSUCHCHANNEL, "TOPIC",
					channel_name + " :No such channel");
		return ;
	}
	if (params.size() == 1)
	{
		if (!ch->isClientInChannel(client))
		{
			if (index != -1)
				_server->sendErrorMessage(index, ERR_NOTONCHANNEL, "TOPIC",
						channel_name + " :You're not on that channel");
			return ;
		}
		std::string	topic = ch->getTopic();
		if (topic.empty())
		{
			_server->sendMessageToClient(client, ":ircserv " + intToString(RPL_NOTOPIC) +
					" " + client->getNick() + " " + channel_name + " :No topic is set\r\n");
		}
		else
		{
			_server->sendMessageToClient(client,
					":ircserv " + intToString(RPL_TOPIC) + " " +
					client->getNick() + " " + channel_name + " :" + topic + "\r\n");
		}
		return ;
	}
	if (!ch->isClientInChannel(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NOTONCHANNEL, "TOPIC",
					channel_name + " :You're not on that channel");
		return ;
	}
	if (ch->hasMode('t') && !ch->isOperator(client))
	{
		if (index != -1)
			_server->sendErrorMessage(index, ERR_CHANOPRIVSNEEDED, "TOPIC",
					channel_name + " :You're not channel operator");
		return ;
	}
	std::string	new_topic = params[1];
	if (!new_topic.empty() && new_topic[0] == ':')
		new_topic = new_topic.substr(1);
	ch->setTopic(new_topic);
	std::string topic_msg = ":" + client->_nickname + "!" + client->_username +
		"@" + client->getHost() + " TOPIC " + channel_name +
		" :" + new_topic + "\r\n";
	ch->broadcastMessage(topic_msg, client);
	_server->sendMessageToClient(client, topic_msg);
}
