/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:43:15 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:31 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/User.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <string>
#include <unistd.h>

User::User(Server *server) : Command(server)
{
}

User::~User()
{
}

void	User::execute(Client *client, const std::vector<std::string> &params)
{
	if (client->_registered)
	{
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->sendErrorMessage(index, ERR_ALREADYREGISTERED, "USER",
					" :You may not reregister");
		return ;
	}
	if (params.size() < 4)
	{
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, "USER",
					" :Not enough parameters");
		return ;
	}
	client->_username = params[0];
	client->_realname = params[3];
	if (client->_nickname.empty())
	{
		return;
	}
	if (!client->_pass_sent)
	{
		std::string msg = ":server " + intToString(ERR_PASSWDMISMATCH) +
			" * :Password required\r\nERROR :Closing Link\r\n";
		_server->sendMessageImmediately(client, msg);
		usleep(100000);
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->closeFd(index, client->fd);
		return ;
	}
	if (client->_bad_password)
	{
		std::string msg = ":server " + intToString(ERR_PASSWDMISMATCH) +
			" * :Password must be sent first\r\nERROR :Closing Link\r\n";
		_server->sendMessageImmediately(client, msg);
		usleep(100000);
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->closeFd(index, client->fd);
		return ;
	}
	if (!client->_is_password)
	{
		std::string msg = ":server " + intToString(ERR_PASSWDMISMATCH) +
			" * :Password incorrect\r\nERROR :Closing Link\r\n";
		_server->sendMessageImmediately(client, msg);
		usleep(100000);
		int	index = _server->getClientIndex(client);
		if (index != -1)
			_server->closeFd(index, client->fd);
		return ;
	}
	if (client->_auth_error_code != 0)
	{
		std::string msg = ":server " + intToString(client->_auth_error_code) +
			" * " + client->_auth_error_msg + "\r\n";
		_server->sendMessageToClient(client, msg);
		return ;
	}
	if (client->_registered)
	{
		return ;
	}
	client->_registered = true;
	std::string	welcome_msg = ":ircserv " + intToString(RPL_WELCOME) + " " + client->_nickname +
		" :Welcome to the ircserv Network, " + client->_nickname + "!" +
		client->_username + "@" + client->getHost() + "\r\n";
	std::string	print_host = ":ircserv " + intToString(RPL_YOURHOST) + " " +
		client->_nickname + " :Your host is ircserv, running version 1.0\r\n";
	std::string	create_msg = ":ircserv " + intToString(RPL_CREATED) + " " + client->_nickname +
		" :This server was created today\r\n";
	std::string	info_msg = ":ircserv " + intToString(RPL_MYINFO) + " " +
		client->_nickname + " ircserv 1.0 i t k o l\r\n";
	_server->sendMessageToClient(client, welcome_msg);
	_server->sendMessageToClient(client, print_host);
	_server->sendMessageToClient(client, create_msg);
	_server->sendMessageToClient(client, info_msg);
}
