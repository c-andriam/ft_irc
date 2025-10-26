/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:29:10 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:45 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Kick : public Command
{
	public:
		Kick(Server* server);
		virtual ~Kick();
		
		void	execute(Client* client, const std::vector<std::string>& params);
};
