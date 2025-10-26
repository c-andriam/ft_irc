/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:31:19 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:00 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Ping : public Command
{
	public:
		Ping(Server* server);
		virtual ~Ping();
		
		void	execute(Client* client, const std::vector<std::string>& params);
};
