/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 19:21:30 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:03:25 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <cstddef>
# include <string>
# include <vector>
# include "../Client.hpp"

class Client;
class Server;

class Command
{
	protected:
		Server*	_server;

	public:
		std::string					name;
		std::vector<std::string>	params;
		Client*						client;

		Command( Server* server );
		virtual ~Command();

		bool			hasEnoughParams( const std::vector<std::string>& params, size_t min_count ) const;
		virtual void	execute( Client* client, const std::vector<std::string>& param ) = 0;
};

#endif
