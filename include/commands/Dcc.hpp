/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dcc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 08:23:02 by candriam          #+#    #+#             */
/*   Updated: 2025/10/27 08:24:23 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <vector>
# include "Command.hpp"

class Dcc : public Command
{
	public:
		Dcc( Server* server );
		virtual ~Dcc();

		void	execute( Client* client, const std::vector<std::string>& params );
};
