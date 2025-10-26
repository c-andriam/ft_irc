/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 04:08:26 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:20 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ErrorCodes.h"
# include <ctime>
# include <string>

# define MAX_FLOOD 3
# define MAX_BUFFER_SIZE 8172

class Server;

class Client
{
	public:
		int			fd;
		int			_flood_count;
		std::string	in_buffer;
		std::string	out_buffer;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_host;
		bool		_bad_password;
		bool		_registered;
		Server		*_server;
		time_t		_last_activity;
		std::string	_expected_pong_token;
		bool		_is_password;
		int			_pass_counter;
		bool		_pass_sent;
		bool		_nick_sent;
		int			_auth_error_code;
		std::string	_auth_error_msg;

		Client();
		Client(int fd);
		~Client();

		std::string	getNick() const;
		Server		*getServer() const;
		void		setServer(Server *server);
		std::string	getHost() const;
		std::pair<IRCErrorCode, std::string>	checkFlood(const std::string &command_line, int fd);
		bool		accumulateBuffer(const std::string &data);
		void		updateActivity();
};

#endif
