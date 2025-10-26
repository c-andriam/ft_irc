/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:32:34 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:06 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Privmsg : public Command
{
	public:
		Privmsg(Server* server);
		virtual ~Privmsg();
		
		void	execute(Client* client, const std::vector<std::string>& params);
};
