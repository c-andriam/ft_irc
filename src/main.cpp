/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 02:44:54 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:06:08 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include "../include/ErrorCodes.h"
#include "../include/Server.hpp"
#include "../include/Structures.h"

volatile std::sig_atomic_t	g_running = 1;

int	isValidPort( const std::string& port )
{
	if (port.empty())
		return (-1);
	for (size_t i = 0; i < port.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(port[i])))
			return (-1);
	}
	if (port.size() > 5)
		return (-1);
	std::istringstream	iss(port);
	long	port_lng;
	if (!(iss >> port_lng) || !iss.eof())
		return (-1);
	if (port_lng < 1024 || port_lng > 65535)
		return (-1);
	return (static_cast<int>(port_lng));
}

bool	isValidPass( const std::string& pass )
{
	if (pass.empty())
		return (false);
	if (pass.size() < 8)
		return (false);
	for (size_t	i = 0; i < pass.size(); ++i)
	{
		if (std::isspace(static_cast<unsigned char>(pass[i])))
			return (false);
	}
	return (true);
}

IRCErrorCode	checkInputArgument(int argc, char *argv[], ServerConfig& config)
{
	if (argc != 3)
		return (IRC_ERR_INVALID_USAGE);
	config.port = isValidPort(argv[1]);
	if (config.port == -1)
		return (IRC_ERR_INVALID_PORT);
	config.password = argv[2];
	if (!isValidPass(config.password))
		return (IRC_ERR_INVALID_PASS);
	return (IRC_SUCCESS);
}

void	handle_sigint(int sig)
{
	(void) sig;
	g_running = 0;
	std::cout << std::endl << "Shutting down server..." << std::endl;
}

int	main(int argc, char *argv[])
{
	std::signal(SIGINT, handle_sigint);
	std::signal(SIGPIPE, SIG_IGN);
	ServerConfig	config;
	IRCErrorCode	code = checkInputArgument(argc, argv, config);
	if (code != IRC_SUCCESS)
	{
		std::cerr << "Error: " << errorCodeToString(code) << std::endl;
		return (IRC_FAIL);
	}
	std::cout << "Starting IRC server..." << std::endl;
	std::cout << "Port: " << config.port << std::endl;
	std::cout << "Password: " << std::string(config.password.size(), '*') << std::endl;
	std::cout << "Server running..." << std::endl;
	try
	{
		Server	server(config.port, config.password);
		server.run();
		std::cout << "Server stopped gracefully" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return (IRC_ERR_RUNTIME);
	}
	return (IRC_SUCCESS);
}
