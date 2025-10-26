/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 04:14:08 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:39 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include <cstddef>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>

Client::Client() :
	fd(-1),
	_flood_count(0),
	in_buffer(""),
	out_buffer(""),
	_nickname(""),
	_username(""),
	_realname(""),
	_bad_password(false),
	_registered(false),
	_last_activity(time(NULL)),
	_expected_pong_token(""),
	_is_password(false),
	_pass_counter(0),
	_pass_sent(false),
	_nick_sent(false),
    _auth_error_code(0),
	_auth_error_msg("")
{
	time(NULL);
}

Client::Client(int fd) :
	fd(fd),
	_flood_count(0),
	in_buffer(""),
	out_buffer(""),
	_nickname(""),
	_username(""),
	_realname(""),
	_bad_password(false),
	_registered(false),
	_last_activity(time(NULL)),
	_expected_pong_token(""),
	_is_password(false),
	_pass_counter(0),
	_pass_sent(false),
	_nick_sent(false),
	_auth_error_code(0),
	_auth_error_msg("")
{
}

Client::~Client()
{
}

std::string	Client::getNick() const
{
	return (_nickname);
}

Server	*Client::getServer() const
{
	return (_server);
}

std::string	Client::getHost() const
{
	return (_host);
}

void	Client::setServer(Server *server)
{
	_server = server;
}

void	Client::updateActivity()
{
	_last_activity = time(NULL);
}

std::pair<IRCErrorCode, std::string>	Client::checkFlood(const std::string &command_line, int fd)
{
	if (command_line.size() > 510)
	{
		_flood_count++;
		std::stringstream ss;
		ss << fd;
		std::string id = ss.str();
		if (_flood_count >= 3)
		{
			std::string warning = ":ircserv NOTICE " + id +
				" :Flood detected. Disconnecting for spamming.\r\n";
			return (std::make_pair(IRC_DISCONNECT, warning));
		}
		else
		{
			std::stringstream ss1;
			ss1 << (3 - _flood_count);
			std::string count = ss1.str();
			std::string warning = ":ircserv NOTICE " + id + " : Warning: Command too long. " + count +
				" more violations will result in disconnection.\r\n";
			return (std::make_pair(IRC_CONTINUE_LOOP, warning));
		}
	}
	_flood_count = 0;
	return (std::make_pair(IRC_SUCCESS, ""));
}

bool	Client::accumulateBuffer(const std::string &data)
{
	if (in_buffer.size() >= MAX_BUFFER_SIZE)
		return (true);
	in_buffer += data;
	return (false);
}
