/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dcc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 08:19:49 by candriam          #+#    #+#             */
/*   Updated: 2025/10/27 08:42:49 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Dcc.hpp"
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

Dcc::Dcc(Server *server) : Command(server)
{
}

Dcc::~Dcc()
{
}

void	Dcc::execute(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	std::cout << "Command known now" << std::endl;
}
