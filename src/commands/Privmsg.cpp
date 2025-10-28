/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:41:47 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 17:45:26 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Privmsg.hpp"
#include "../../include/Channel.hpp"
#include "../../include/IrcReplies.h"
#include "../../include/Server.hpp"
#include "../../include/Utils.hpp"
#include <cstddef>
#include <execution>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

Privmsg::Privmsg(Server *server) : Command(server)
{
}

Privmsg::~Privmsg()
{
}

bool	isClientToClientProtocol(const std::string& msg)
{
	if (msg.empty())
		return (false);
	std::string	prefix = "\x01" "DCC ";
	std::string	suffix = "\x01";
	bool	start = false;
	bool	end = false;
	if (msg.size() >= prefix.size())
		start = (msg.substr(0, prefix.size()) == prefix);
	if (msg.size() >= suffix.size())
		end = (msg.substr(msg.size() - suffix.size()) == suffix);
	if (start && end)
		return (true);
	return (false);
}

bool	validFileSize(std::string str_size)
{
	for (size_t i = 0; i < str_size.size(); ++i)
		if (!std::isdigit(str_size[i]))
			return (false);
	long	long_size;
	std::istringstream	iss(str_size);
	iss >> long_size;
	if (iss.fail() || iss.eof())
		return (false);
	if (long_size == 0)
		return (false);
	long size_max = 2147483648;
	if (long_size > size_max)
		return (false);
	return (true);
}

std::string	extractMessageCTCP(const std::string& msg)
{
	if (isClientToClientProtocol(msg))
	{
		std::string	prefix = "\x01" "DCC ";
		std::string	suffix = "\x01";
		std::string	start = msg.substr(0, prefix.size());
		std::string	end = msg.substr(msg.size() - suffix.size());
		std::string	extracted_msg = msg.substr(prefix.size(), msg.size() - prefix.size() - suffix.size());
		return (extracted_msg);
	}
	std::string	result = "";
	return (result);
}

std::vector<std::string>	splitParams(const std::string& extracted_msg)
{
	std::vector<std::string>	params;
	std::istringstream	iss(extracted_msg);
	std::string	token;
	while (iss >> token)
		params.push_back(token);
	return (params);
}

bool	isValidDcc(const std::vector<std::string>& params, std::string& error_msg)
{
	if (params.size() != 5)
	{
		error_msg = "Invalid DCC SEND format (expected 5 parameters)";
		return (false);
	}
	if (params[0] != "SEND")
	{
		error_msg = "Not a DCC SEND command";
		return (false);
	}
	const std::string& filename = params[1];
	if (filename.find("..") != std::string::npos || filename.find("/") != std::string::npos || filename.find("\\") != std::string::npos)
	{
		error_msg = "Dangerous filename (path traversal attempt)";
		return (false);
	}
	const std::string&	ip_str = params[2];
	for (size_t i = 0; i < ip_str.length(); ++i)
	{
		if (!std::isdigit(ip_str[i]))
		{
			error_msg = "Invalid IP format (must be decimal)";
			return (false);
		}
	}
	const std::string&	port_str = params[3];
	for (size_t i = 0; i < port_str.length(); ++i)
	{
		if (!std::isdigit(port_str[i]))
		{
			error_msg = "Invalid port format";
			return (false);
		}
	}
	std::istringstream	port_iss(port_str);
	int	port;
	port_iss >> port;
	if (port_iss.fail() || !port_iss.eof())
	{
		error_msg = "Invalid port value";
		return (false);
	}
	if (port < 1024 || port > 65535)
	{
		error_msg = "Port value out of range (1024 - 65535)";
		return (false);
	}
	const std::string&	size_str = params[4];
	for (size_t i = 0; i < size_str.length(); ++i)
	{
		if (!std::isdigit(size_str[i]))
		{
			error_msg = "Invalid size value";
			return (false);
		}
	}
	std::istringstream	size_iss(size_str);
	long long	size;
	size_iss >> size;
	if (size_iss.fail() || !size_iss.eof())
	{
		error_msg = "Invalid size value";
		return (false);
	}
	if (size > 2147483648LL)
	{
		error_msg = "File too large";
		return (false);
	}
	std::cout << params[0] << std::endl << params[1] << std::endl << params[2] << std::endl << params[3] << std::endl << params[4] << std::endl;
	return (true);
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
		"@" + client->getHost() + " PRIVMSG " + target + " :" +
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
		if (isClientToClientProtocol(msg))
		{
			std::string	extracted_msg = extractMessageCTCP(msg);
			std::string	privmsg = ":" + client->_nickname + "!" + client->_username +
			"@" + client->getHost() + " PRIVMSG " + target + " :" +
			msg + "\r\n";
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
			//std::cout << "[" << extracted_msg << "]" << std::endl;
			std::vector<std::string>	dcc = splitParams(extracted_msg);
			std::string	error_msg;
			if (isValidDcc(dcc, error_msg))
			{
				std::cout << "DCC SEND VALID" << std::endl;
			}
		}
		else
		{
			Client	*targetClient = _server->getClientByNick(target);
			if (targetClient)
			{
				std::cout << "ATO" << std::endl;
				_server->sendMessageToClient(targetClient, privmsg);
			}
			else
			{
				int	i = _server->getClientIndex(client);
				if (i != -1)
					_server->sendErrorMessage(i, ERR_NOSUCHNICK, "PRIVMSG",
							target + " :No such nick/channel");
			}
		}
	}
}
