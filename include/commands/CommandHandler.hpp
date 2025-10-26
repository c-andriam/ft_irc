/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 17:52:26 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:31 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include <vector>
#include "Command.hpp"
#include "IrcCommands.h"

class Command;
class Client;
class Server;

class CommandHandler
{
	public:
		CommandHandler(Server* server);
		~CommandHandler();

		void		registerCommand(IRCCommands cmdEnum, Command* cmd);
		void		handleCommand(const std::string& name, Client* client,
							const std::vector<std::string>& params);
		void		handleCommandEnum(Client* client, IRCCommands cmdEnum, const std::vector<std::string>& params);

		void		handleUnknownCommand(Client* client, const std::string& cmd);
		void		handleNotEnoughParams(Client* client, const std::string& cmd);
		void		handleNotRegistered(Client* client);
		std::string	getCommandString(IRCCommands cmdEnum);

	private:
		Server*							_server;
		std::map<IRCCommands, Command*> _commands;
		
		std::string	toUpper(const std::string& str) const;
		IRCCommands	getCommandEnum(const std::string& command);
};
