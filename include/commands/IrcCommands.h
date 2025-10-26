/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCommands.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 21:10:57 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:38 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_COMMANDS_H
# define IRC_COMMANDS_H

#include <string>

enum	IRCCommands
{
	PASS_COMMAND = 1,
	NICK_COMMAND,
	USER_COMMAND,
	PRIVMSG_COMMAND,
	JOIN_COMMAND,
	TOPIC_COMMAND,
	KICK_COMMAND,
	INVITE_COMMAND,
	MODE_COMMAND,
	PING_COMMAND,
	PONG_COMMAND,
	CAP_COMMAND,
};

std::string	ircCommands(IRCCommands command);
IRCCommands	getCommandEnum(const std::string& command);

# endif
