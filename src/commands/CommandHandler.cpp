/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 22:10:03 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:54 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/CommandHandler.hpp"
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/IrcReplies.h"
#include <cctype>
#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <iostream>

CommandHandler::CommandHandler(Server* server) : _server(server)
{
}

CommandHandler::~CommandHandler()
{
	std::map<IRCCommands, Command*>::iterator it;
	for (it = _commands.begin(); it != _commands.end(); ++it)
		delete it->second;
	_commands.clear();
}

void	CommandHandler::registerCommand(IRCCommands cmdEnum, Command* cmd)
{
	_commands[cmdEnum] = cmd;
}


std::string	CommandHandler::getCommandString(IRCCommands cmdEnum)
{
	switch (cmdEnum)
	{
		case JOIN_COMMAND:
			return ("JOIN");
		case NICK_COMMAND:
			return ("NICK");
		case PASS_COMMAND:
			return ("PASS");
		case USER_COMMAND:
			return ("USER");
		case PRIVMSG_COMMAND:
			return ("PRIVMSG");
		case PING_COMMAND:
			return ("PING");
		case PONG_COMMAND:
			return ("PONG");
		case MODE_COMMAND:
			return ("MODE");
		case TOPIC_COMMAND:
			return ("TOPIC");
		case KICK_COMMAND:
			return ("KICK");
		case INVITE_COMMAND:
			return ("INVITE");
		default:
			return ("Not a valid command");
	}
}

void	CommandHandler::handleCommand(const std::string& name, Client* client, const std::vector<std::string>& params)
{
	IRCCommands	cmdEnum = getCommandEnum(name);
	if (cmdEnum == 0)
	{
		handleUnknownCommand(client, name);
		return ;
	}
	handleCommandEnum(client, cmdEnum, params);
}

void	CommandHandler::handleCommandEnum(Client* client, IRCCommands cmdEnum, const std::vector<std::string>& params)
{
	std::map<IRCCommands, Command*>::iterator it = _commands.find(cmdEnum);
	if (it != _commands.end())
		it->second->execute(client, params);
	else
		handleUnknownCommand(client, getCommandString(cmdEnum));
}

std::string	CommandHandler::toUpper(const std::string& str) const
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
		result[i] = toupper(result[i]);
	return (result);
}

void	CommandHandler::handleUnknownCommand(Client* client, const std::string& cmd)
{
	int index = _server->getClientIndex(client);
	if (index != -1)
		_server->sendErrorMessage(index, ERR_UNKNOWNCOMMAND, cmd, ":Unknown command");
}

void	CommandHandler::handleNotEnoughParams(Client* client, const std::string& cmd)
{
	int index = _server->getClientIndex(client);
	if (index != -1)
		_server->sendErrorMessage(index, ERR_NEEDMOREPARAMS, cmd, ":Not enough parameters");
}

void	CommandHandler::handleNotRegistered(Client* client)
{
	int index = _server->getClientIndex(client);
	if (index != -1)
		_server->sendErrorMessage(index, ERR_NOTREGISTERED, "", ":You have not registered");
}

IRCCommands	CommandHandler::getCommandEnum(const std::string& command)
{
	std::string upper = toUpper(command);
	if (upper == "PASS")
		return (PASS_COMMAND);
	if (upper == "CAP")
		return (CAP_COMMAND);
	if (upper == "NICK")
		return (NICK_COMMAND);
	if (upper == "USER")
		return (USER_COMMAND);
	if (upper == "PRIVMSG")
		return (PRIVMSG_COMMAND);
	if (upper == "JOIN")
		return (JOIN_COMMAND);
	if (upper == "PING")
		return (PING_COMMAND);
	if (upper == "PONG")
		return (PONG_COMMAND);
	if (upper == "MODE")
		return (MODE_COMMAND);
	if (upper == "TOPIC")
		return (TOPIC_COMMAND);
	if (upper == "KICK")
		return (KICK_COMMAND);
	if (upper == "INVITE")
		return (INVITE_COMMAND);
	return (IRCCommands)0;
}
