/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 22:36:00 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:50 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/Command.hpp"

Command::Command(Server* server) : _server(server)
{
}

Command::~Command()
{
}

bool	Command::hasEnoughParams(const std::vector<std::string>& params, size_t minCount) const
{
	return (params.size() >= minCount);
}
