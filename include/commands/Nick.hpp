/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:30:56 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:54 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Nick : public Command
{
	public:
		Nick(Server* server);
		virtual ~Nick();
		
		void	execute(Client* client, const std::vector<std::string>& params);
};
