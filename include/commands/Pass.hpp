/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 20:53:34 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:57 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Pass : public Command
{
	public:
		Pass(Server* server);
		virtual ~Pass();
		
		void	execute(Client* client, const std::vector<std::string>& params);
};
