/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:28:29 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:42 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <vector>
# include "Command.hpp"

class Join : public Command
{
	public:
		Join( Server* server );
		virtual ~Join();

		void	execute( Client* client, const std::vector<std::string>& params );
};
