/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 01:19:47 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:54 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Reply.hpp"
#include "../include/IrcReplies.h"
#include "../include/Utils.hpp"
#include <sstream>
#include <string>
#include <vector>

std::vector<Reply::ReplyFormat>	Reply::_reply_formats;
bool	Reply::_initialized = false;

void	Reply::initReplyFormats()
{
	if (_initialized)
		return ;
	Reply::ReplyFormat format;
	format.code = RPL_WELCOME;
	format.format = ":Welcome to the IRC %s";
	_reply_formats.push_back(format);
	format.code = ERR_UNKNOWNCOMMAND;
	format.format = "%s :Unknown command";
	_reply_formats.push_back(format);
	format.code = ERR_NOSUCHNICK;
	format.format = " :You need a nickname";
	_reply_formats.push_back(format);
	format.code = ERR_NEEDMOREPARAMS;
	format.format = "%s :Not enough parameters";
	_reply_formats.push_back(format);
	format.code = ERR_NOTREGISTERED;
	format.format = ":You have not registred";
	_reply_formats.push_back(format);
	_initialized = true;
}

void	Reply::sendReply(Client *client, int code, const std::string &msg)
{
	std::string	serverName = "ircserv";
	std::string	nickname = "*";
	if (client->_nickname.length() > 0)
		nickname = client->_nickname;
	std::string	formatReply = ":" + serverName + " " + intToString(code) + " " +
		nickname + " " + msg + "\r\n";
	client->out_buffer += formatReply;
}
