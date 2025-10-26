/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 22:41:24 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:47 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Cap.hpp"
#include "../../include/Server.hpp"
#include <string>

Cap::Cap(Server* server) : Command(server)
{
}

Cap::~Cap()
{
}

void	Cap::execute(Client* client, const std::vector<std::string>& params)
{
	if (!params.empty())
	{
		if (params[0] == "LS")
		{
			std::string	answer = ":ircserver CAP * LS :\r\n";
			_server->sendMessageToClient(client, answer);
		}
		else if (params[0] == "END")
		{
		}
	}
}
