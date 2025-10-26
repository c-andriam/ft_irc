/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:31:50 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:03 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Pong : public Command
{
	public:
		Pong(Server* server);
		virtual ~Pong();
	
		void	execute(Client* client, const std::vector<std::string>& params);
};
