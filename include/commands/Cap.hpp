/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cap.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 20:53:34 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:08:46 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Cap : public Command
{
	public:
		Cap(Server* server);
		virtual ~Cap();
		
		void execute(Client* client, const std::vector<std::string>& params);
};
