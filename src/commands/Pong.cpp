/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:41:23 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:21 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Pong.hpp"
#include "../../include/Server.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Utils.hpp"
#include <string>
#include <iostream>

Pong::Pong(Server* server) : Command(server)
{
}

Pong::~Pong()
{
}

void	Pong::execute(Client* client, const std::vector<std::string>& params)
{
	if (params.empty())
	{
		_server->sendMessageToClient(client, ":ircserv " + intToString(ERR_NEEDMOREPARAMS) + client->getNick() + " PONG :Not enough parameters\r\n");
		std::cout << "ERR_NEEDMOREPARAMS sent to " << client->getNick() << std::endl;
		return ;
	}
	else
	{
		std::string received_token = params[0];
		if (!received_token.empty() && received_token[0] == ':')
			received_token = received_token.substr(1);
		if (client->_expected_pong_token == received_token)
		{
			client->updateActivity();
			std::cout << "PONG received and activity updated for " << client->getNick() << std::endl;
		}
		else
		{
			std::cout << "Invalid PONG token from " << client->getNick() << " expected: " << client->_expected_pong_token << " received: " << received_token << std::endl;
		}
	}
}
