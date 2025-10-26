/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCodes.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 03:28:49 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:05:42 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ErrorCodes.h"
#include <string>

std::string	errorCodeToString(IRCErrorCode code)
{
	switch (code)
	{
		case IRC_SUCCESS:
			return ("");
		case IRC_ERR_INVALID_USAGE:
			return ("Invalid usage: ./ircserv <port> <password>");
		case IRC_ERR_INVALID_PORT:
			return ("Port input error: port must be a number at least [1024 - 65535]");
		case IRC_ERR_EMPTY_PASS:
			return ("Password cannot be empty");
		case IRC_ERR_INVALID_PASS:
			return ("Password input error: password MUST contain >= 8 characters");
		case IRC_ERR_PARSE:
			return ("Parsing error: too many/few arguments provided");
		case IRC_ERR_SOCKET:
			return ("Fail to create the listening Socket");
		case IRC_ERR_RUNTIME:
			return ("failed to run server");
		case IRC_FAIL:
			return ("General error");
		default:
			return ("Unknown error");
	}
}
