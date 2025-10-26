/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCodes.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 03:10:01 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:23 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORCODES_H
# define ERRORCODES_H

# include <string>

# define JOIN_MSG " has join the chat group!"
# define LEFT_MSG " has left the chat group!"

enum	IRCErrorCode
{
	IRC_SUCCESS = 0,
	IRC_ERR_INVALID_USAGE,
	IRC_ERR_INVALID_PORT,
	IRC_ERR_EMPTY_PASS,
	IRC_ERR_INVALID_PASS,
	IRC_ERR_PARSE,
	IRC_ERR_SOCKET,
	IRC_BREAK_LOOP,
	IRC_CONTINUE_LOOP,
	IRC_DISCONNECT,
	IRC_FAIL,
	IRC_ERR_RUNTIME,
};

std::string	errorCodeToString(IRCErrorCode code);

#endif
