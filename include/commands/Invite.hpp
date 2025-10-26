/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:25:50 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:35 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <vector>
# include "Command.hpp"

class Invite : public Command
{
	public:
		Invite( Server* server );
		virtual ~Invite();

		void	execute( Client* client, const std::vector<std::string>& params );
};
