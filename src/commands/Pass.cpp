/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 22:41:24 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:15 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Pass.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

Pass::Pass(Server *server) : Command(server)
{
}

Pass::~Pass()
{
}

void	Pass::execute(Client *client, const std::vector<std::string> &params)
{
	if (client->_pass_sent)
	{
		std::string msg = ":server " + intToString(ERR_ALREADYREGISTERED);
		if (!client->_nickname.empty())
			msg += client->_nickname;
		else
			msg += "*";
		msg += " :You may not reregister\r\n";
		_server->sendMessageToClient(client, msg);
		return ;
	}
	if (params.empty())
	{
		std::string msg = ":server " + intToString(ERR_NEEDMOREPARAMS) +
			" * PASS :Not enough paramaters\r\n";
		_server->sendMessageToClient(client, msg);
		return ;
	}
	if (client->_nick_sent)
	{
		client->_bad_password = true;
	}
	client->_pass_sent = true;
	if (params[0] != _server->getPassword())
	{
		client->_is_password = false;
	}
	else
	{
		client->_is_password = true;
	}
	client->_pass_counter = 0;
	std::cout << "Client fd=" << client->fd << " authenticated successfully"
		<< std::endl;
}
