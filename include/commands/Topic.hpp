/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:33:18 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:09 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Command.hpp"
# include <string>
# include <vector>

class Topic : public Command
{
	private:
		bool	validateParams(const std::vector<std::string> &params,
				std::string &channel_name, std::string &topic, Client *client);

	public:
		Topic(Server *server);
		virtual ~Topic();

		void	execute(Client *client, const std::vector<std::string> &params);
};
