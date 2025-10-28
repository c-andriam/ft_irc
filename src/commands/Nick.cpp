/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:38:57 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/27 07:14:58 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Nick.hpp"
#include "../../include/Client.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <string>
#include <unistd.h>

Nick::Nick(Server *server) : Command(server)
{
}

Nick::~Nick()
{
}

static bool	isValidNickname(const std::string &nick)
{
	if (nick.empty() || nick.length() > 9)
		return (false);
	char	first = nick[0];
	if (!((first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z') ||
        first == '[' || first == ']' || first == '\\' || first == '`' ||
        first == '_' || first == '^' || first == '{' || first == '|' ||
        first == '}'))
		return (false);
	for (size_t i = 1; i < nick.length(); ++i)
	{
		char	c = nick[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == '[' || c == ']' || c == '\\' ||
          c == '`' || c == '_' || c == '^' || c == '{' || c == '|' ||
          c == '}' || c == '-'))
			return (false);
	}
	return (true);
}

void	Nick::execute(Client *client, const std::vector<std::string> &params)
{
	client->_nick_sent = true;
	if (client->_registered)
	{
		if (params.empty())
		{
			int	index = _server->getClientIndex(client);
			if (index != -1)
				_server->sendErrorMessage(index, ERR_NONICKNAMEGIVEN, "NICK",
						" :No nickname given");
			return ;
		}
		std::string	new_nick = params[0];
		if (!isValidNickname(new_nick))
		{
			int	index = _server->getClientIndex(client);
			if (index != -1)
				_server->sendErrorMessage(index, ERR_ERRONEUSNICKNAME, "NICK",
						new_nick + " :Erroneous nickname");
			return ;
		}
		Client	*exist_nick = _server->getClientByNick(new_nick);
		if (exist_nick && exist_nick != client)
		{
			int	index = _server->getClientIndex(client);
			if (index != -1)
				_server->sendErrorMessage(index, ERR_NICKNAMEINUSE, "NICK",
						new_nick + " :Nickname is already in use");
			return ;
		}
		std::string	old_nick = client->_nickname;
		client->_nickname = new_nick;
		std::string change_msg = ":" + old_nick + "!" + client->_username + "@" +
			client->getHost() + " NICK :" + new_nick + "\r\n";
		_server->sendMessageToClient(client, change_msg);
		return ;
	}
	if (params.empty())
	{
		client->_auth_error_code = ERR_NONICKNAMEGIVEN;
		client->_auth_error_msg = " :No nickname given";
		client->_nickname = "*";
		return ;
	}
	std::string	new_nick = params[0];
	client->_nickname = new_nick;
	Client	*exist_nick = _server->getClientByNick(new_nick);
	if (exist_nick && exist_nick != client)
	{
		client->_auth_error_code = ERR_NICKNAMEINUSE;
		client->_auth_error_msg = new_nick + " :Nickname is already in use";
		return ;
	}
	if (!isValidNickname(new_nick))
	{
		client->_auth_error_code = ERR_ERRONEUSNICKNAME;
		client->_auth_error_msg = new_nick + " :Erroneous nickname";
		return ;
	}
	if (!client->_username.empty() && !client->_registered)
	{
		if (!client->_pass_sent)
		{
			std::string msg = ":ircserv " + intToString(ERR_PASSWDMISMATCH) +
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
			std::string msg = ":ircserv " + intToString(ERR_PASSWDMISMATCH) +
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
			std::string msg = ":ircserv " + intToString(ERR_PASSWDMISMATCH) +
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
			std::string msg = ":ircserv " + intToString(client->_auth_error_code) +
				" * " + client->_auth_error_msg + "\r\n";
			_server->sendMessageToClient(client, msg);
			return ;
		}
		client->_registered = true;
		std::string welcome_msg =
			":ircserv " + intToString(RPL_WELCOME) + " " + client->_nickname +
			" :Welcome to the ircserv Network, " + client->_nickname + "!" +
			client->_username + "@" + client->getHost() + "\r\n";
		std::string print_host = ":ircserv " + intToString(RPL_YOURHOST) + " " +
			client->_nickname + " :Your host is ircserv, running version 1.0\r\n";
		std::string create_msg = ":ircserv " + intToString(RPL_CREATED) + " " +
			client->_nickname + " :This server was created today\r\n";
		std::string info_msg = ":ircserv " + intToString(RPL_MYINFO) + " " +
			client->_nickname + " ircserv 1.0 i t k o l\r\n";
		_server->sendMessageToClient(client, welcome_msg);
		_server->sendMessageToClient(client, print_host);
		_server->sendMessageToClient(client, create_msg);
		_server->sendMessageToClient(client, info_msg);
	}
}
