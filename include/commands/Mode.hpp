/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:30:14 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:49 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "../Channel.hpp"
# include "Command.hpp"
# include <string>
# include <vector>

class Mode : public Command
{
	private:
		bool	validateParams(const std::vector<std::string> &params,
				std::string &channel_name, std::string &mode,
				std::string &param, Client *client);
		
		bool	checkPermissions(Client *client, Channel *channel);
		bool	parseMode(const std::string &mode, char &char_mode, bool &is_adding,
				std::string &param, Client *client, Channel *channel);

		void	handleModeChange(Channel *channel, char char_mode, bool is_adding,
				const std::string &param_value);

	public:
		Mode(Server *server);
		virtual ~Mode();

		void	execute(Client *client, const std::vector<std::string> &params);
};
