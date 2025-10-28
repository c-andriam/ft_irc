/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:40:51 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/27 07:02:12 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Ping.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <iostream>
#include <string>

Ping::Ping(Server *server) : Command(server)
{
}

Ping::~Ping()
{
}

void	Ping::execute(Client *client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		_server->sendMessageToClient(client, ":ircserv " + intToString(ERR_NEEDMOREPARAMS) +
				client->getNick() + " PING :Not enough parameters\r\n");
		std::cout << "ERR_NEEDMOREPARAMS sent to " << client->getNick()
			<< std::endl;
		return ;
	}
	else
	{
		std::string token = params[0];
		std::string pong_msg = ":ircserv PONG ircserv :" + token + "\r\n";
		_server->sendMessageToClient(client, pong_msg);
		std::cout << "PONG sent to " << client->getNick() << std::endl;
	}
}
