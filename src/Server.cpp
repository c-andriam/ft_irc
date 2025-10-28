/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:04:01 by candriam          #+#    #+#             */
/*   Updated: 2025/10/27 07:01:56 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/ErrorCodes.h"
#include "../include/IrcReplies.h"
#include "../include/Parser.hpp"
#include "../include/Utils.hpp"
#include "../include/commands/Cap.hpp"
#include "../include/commands/Invite.hpp"
#include "../include/commands/Join.hpp"
#include "../include/commands/Kick.hpp"
#include "../include/commands/Mode.hpp"
#include "../include/commands/Nick.hpp"
#include "../include/commands/Pass.hpp"
#include "../include/commands/Ping.hpp"
#include "../include/commands/Pong.hpp"
#include "../include/commands/Privmsg.hpp"
#include "../include/commands/Topic.hpp"
#include "../include/commands/User.hpp"
#include "../include/commands/Dcc.hpp"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <new>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

extern volatile std::sig_atomic_t	g_running;

Server::Server(unsigned int port, const std::string &password) :
	_server_fd(-1),
	_port(port),
	_running(false),
	_last_ping_time(0),
	_password(password),
	_serverName("ircserv"),
	_commandHandler(NULL)
{
	std::memset(&_server_addr, 0, sizeof(_server_addr));
	initClientsArrays();
	initializeCommandHandler();
}

void	Server::initClientsArrays()
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		pollfds[i].fd = -1;
		pollfds[i].events = 0;
		pollfds[i].revents = 0;
		clients[i] = Client();
	}
}

void	Server::initializeCommandHandler()
{
	try
	{
		_commandHandler = new CommandHandler(this);
		registerAllCommands();
	}
	catch (const std::bad_alloc &e)
	{
		cleanupCommandHandler();
		throw std::runtime_error("Memory allocation failed for CommandHandler");
	}
	catch (const std::exception &e)
	{
		cleanupCommandHandler();
		throw std::runtime_error("Failed to initialize CommandHandler: " +
				std::string(e.what()));
	}
}

void	Server::registerAllCommands()
{
	if (!_commandHandler)
	{
		throw std::runtime_error("Cannot register commands: CommandHandler is NULL");
	}
	_commandHandler->registerCommand(PASS_COMMAND, new Pass(this));
	_commandHandler->registerCommand(NICK_COMMAND, new Nick(this));
	_commandHandler->registerCommand(USER_COMMAND, new User(this));
	_commandHandler->registerCommand(PRIVMSG_COMMAND, new Privmsg(this));
	_commandHandler->registerCommand(JOIN_COMMAND, new Join(this));
	_commandHandler->registerCommand(TOPIC_COMMAND, new Topic(this));
	_commandHandler->registerCommand(KICK_COMMAND, new Kick(this));
	_commandHandler->registerCommand(INVITE_COMMAND, new Invite(this));
	_commandHandler->registerCommand(MODE_COMMAND, new Mode(this));
	_commandHandler->registerCommand(PING_COMMAND, new Ping(this));
	_commandHandler->registerCommand(PONG_COMMAND, new Pong(this));
	_commandHandler->registerCommand(CAP_COMMAND, new Cap(this));
	_commandHandler->registerCommand(DCC_COMMAND, new Dcc(this));
}

void	Server::cleanupCommandHandler()
{
	if (_commandHandler)
	{
		delete _commandHandler;
		_commandHandler = NULL;
	}
}

void	Server::initializeServer()
{
	initSocket();
	initPoll();
	_last_ping_time = time(NULL);
	_running = true;
	std::cout << "Server initialized successfully" << std::endl;
}

bool	Server::shouldContinueRunning()
{
	if (!_running)
		return (false);
	if (!g_running)
		return (false);
	return (true);
}

void	Server::handlePing()
{
	const int	PING_INTERVAL = 60;
	time_t	now = time(NULL);
	if (now - _last_ping_time > PING_INTERVAL)
	{
		sendPingToAllClients();
		_last_ping_time = now;
	}
}

void	Server::checkClientTimeout()
{
	const int	CLIENT_TIMEOUT = 300;
	time_t	now = time(NULL);
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd == -1)
			continue ;
		time_t	inactive = now - clients[i]._last_activity;
		if (inactive > CLIENT_TIMEOUT)
		{
			std::cout << "Client timeout: fd=" << pollfds[i].fd << " (inactive for "
				<< inactive << "s)" << std::endl;
			closeFd(i, pollfds[i].fd);
		}
	}
}

bool	Server::shouldBreakLoop(IRCErrorCode code)
{
	if (code == IRC_SUCCESS)
		return (false);
	if (code == IRC_BREAK_LOOP)
		return (true);
	return (false);
}

void	Server::run()
{
	initializeServer();
	while (shouldContinueRunning())
	{
		updatePollEvents();
		handlePing();
		IRCErrorCode code = handlePoll();
		if (shouldBreakLoop(code))
			break;
		checkClientTimeout();
	}
}

void	Server::createServerSocket()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
		throw std::runtime_error("Failed to create socket: " +
				std::string(strerror(errno)));
}

void	Server::configSocketOptions()
{
	const int	opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		close(_server_fd);
		throw std::runtime_error("Failed to set SO_REUSEADDR: " +
				std::string(strerror(errno)));
	}
	if (setsockopt(_server_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Warning: Failed to set TCP_NODELAY" << std::endl;
	}
}

void	Server::setSocketNonBlocking(int fd)
{
	int	flag = fcntl(fd, F_GETFL, 0);
	if (flag == -1)
	{
		close(fd);
		throw std::runtime_error("Failed to get socket flags: " +
				std::string(strerror(errno)));
	}
	if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
	{
		close(fd);
		throw std::runtime_error("Failed to set non-blocking mode: " +
				std::string(strerror(errno)));
	}
}

void	Server::prepareServerAddress()
{
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(_port);
}

void	Server::bindServerSocket()
{
	if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
	{
		close(_server_fd);
		if (errno == EADDRINUSE)
		{
			std::ostringstream oss;
			oss << "Port " << _port << " is already in use";
			throw std::runtime_error(oss.str());
		}
		else if (errno == EACCES)
		{
			throw std::runtime_error("Permission denied (try port > 1024)");
		}
		else
		{
			throw std::runtime_error("Failed to bind socket: " +
					std::string(strerror(errno)));
		}
	}
}

void	Server::listenServerSocket()
{
	if (listen(_server_fd, SOMAXCONN) == -1)
	{
		close(_server_fd);
		throw std::runtime_error("Failed to listen on socket: " +
				std::string(strerror(errno)));
	}
}

void	Server::initSocket()
{
	createServerSocket();
	configSocketOptions();
	setSocketNonBlocking(_server_fd);
	prepareServerAddress();
	bindServerSocket();
	listenServerSocket();
}

int	Server::acceptNewClient(struct sockaddr_in &client_addr)
{
	socklen_t client_len = sizeof(client_addr);
	int	client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (-1);
		std::cerr << "Error: accept() failed: " << strerror(errno) << std::endl;
		return (-1);
	}
	return (client_fd);
}

std::string	Server::extractClientIP(const struct sockaddr_in &addr)
{
	char	ip_str[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN) == NULL)
	{
		std::cerr << "Warning: Failed to convert IP address" << std::endl;
		return ("0.0.0.0");
	}
	return (std::string(ip_str));
}

int	Server::findFreeClientSlot()
{
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd == -1)
			return (i);
	}
	return (-1);
}

void	Server::initializeNewClient(int slot, int client_fd,
		const std::string &ip)
{
	pollfds[slot].fd = client_fd;
	pollfds[slot].events = POLLIN;
	pollfds[slot].revents = 0;
	clients[slot] = Client(client_fd);
	clients[slot]._host = ip;
	clients[slot].setServer((this));
	clients[slot].updateActivity();
}

void	Server::rejectConnection(int client_fd, const std::string &reason)
{
	std::cerr << "Rejecting conection fd=" << client_fd << ": " << reason
		<< std::endl;
	std::string error_msg = "Error :ircserv unavailable: " + reason + "\r\n";
	send(client_fd, error_msg.c_str(), error_msg.size(), 0);
	close(client_fd);
}

void	Server::initPoll()
{
	pollfds[0].fd = getServerFd();
	pollfds[0].events = POLLIN;
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		pollfds[i].fd = -1;
		pollfds[i].events = 0;
	}
	for (int j = 0; j < MAX_CLIENTS; ++j)
		clients[j] = Client();
}

IRCErrorCode	Server::handlePoll()
{
	int	res = callPoll();
	IRCErrorCode	code = handlePollError(res);
	if (code != IRC_SUCCESS)
		return (code);
	processPollEvents();
	return (IRC_SUCCESS);
}

void	Server::sendErrorMessage(int i, int error_code, const std::string &cmd_name, const std::string &msg)
{
	std::string error_msg = ":ircserv " + intToString(error_code) + " " +
		clients[i]._nickname + " " + cmd_name + " " + msg + "\r\n";
	clients[i].out_buffer += error_msg;
	pollfds[i].events |= POLLOUT;
}

void	Server::sendMessageToClient(Client *client, const std::string &msg)
{
	int	index = getClientIndex(client);
	if (index != -1)
	{
		clients[index].out_buffer += msg;
		pollfds[index].events |= POLLOUT;
	}
}

void	Server::sendMessageImmediately(Client *client, const std::string &msg)
{
	int	index = getClientIndex(client);
	if (index == -1)
		return ;
	send(pollfds[index].fd, msg.c_str(), msg.size(), 0);
}

void	Server::handleBufferAccumulation(int i, const std::string &data)
{
	if (clients[i].in_buffer.size() >= MAX_BUFFER_SIZE)
	{
		std::cerr << "Flood detected on fd " << pollfds[i].fd << ", disconecting"
			<< std::endl;
		closeFd(i, pollfds[i].fd);
		return ;
	}
	clients[i].in_buffer += data;
}

void	Server::handleNewConnection()
{
	struct sockaddr_in	client_addr;
	std::memset(&client_addr, 0, sizeof(client_addr));
	int	client_fd = acceptNewClient(client_addr);
	if (client_fd < 0)
		return;
	std::string	ip = extractClientIP(client_addr);
	try
	{
		setSocketNonBlocking(client_fd);
	}
	catch (const std::exception &e)
	{
		rejectConnection(client_fd, "Failed to set non-blocking mode");
		return ;
	}
	int	slot = findFreeClientSlot();
	if (slot == -1)
	{
		rejectConnection(client_fd, "Server full");
		return ;
	}
	initializeNewClient(slot, client_fd, ip);
	std::cout << "New client connected: fd=" << client_fd << " ip=" << ip
		<< " slot=" << slot << std::endl;
}

void	Server::handleGroupMsg(int i, int client_fd, std::string msg)
{
	std::ostringstream	iss;
	iss << client_fd;
	std::string join_id = iss.str();
	std::string join_msg = "User " + join_id + msg + "\r\n";
	for (int j = 1; j < MAX_CLIENTS; ++j)
	{
		if (pollfds[j].fd != -1 && j != i)
		{
			clients[j].out_buffer += join_msg;
			pollfds[j].events |= POLLOUT;
		}
	}
}

void	Server::updatePollEvents()
{
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd != -1)
		{
			pollfds[i].events = POLLIN;
			if (!clients[i].out_buffer.empty())
				pollfds[i].events |= POLLOUT;
		}
	}
}

ssize_t	Server::sendDataToClient(int client_fd, const std::string &data)
{
	ssize_t	bytes_sent = send(client_fd, data.c_str(), data.size(), 0);
	return (bytes_sent);
}

void	Server::updateOutput(std::string &buffer, ssize_t bytes_sent)
{
	buffer.erase(0, bytes_sent);
}

bool	Server::handleSendError(int slot, int client_fd)
{
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return (false);
	if (errno == EPIPE)
	{
		std::cout << "Client closed connection (EPIPE): fd=" << client_fd
			<< std::endl;
		closeFd(slot, client_fd);
		return (true);
	}
	std::cerr << "Error sending to client fd=" << client_fd << ": "
		<< strerror(errno) << std::endl;
	closeFd(slot, client_fd);
	return (true);
}

void	Server::handleClientWrite(int i)
{
	int	client_fd = pollfds[i].fd;
	std::string	&buffer = clients[i].out_buffer;
	if (buffer.empty())
		return;
	ssize_t	bytes_sent = sendDataToClient(client_fd, buffer);
	if (bytes_sent > 0)
	{
		updateOutput(buffer, bytes_sent);
		return ;
	}
	if (bytes_sent < 0)
		handleSendError(i, client_fd);
}

IRCErrorCode	Server::handleFlood(int i, std::string command_line)
{
	std::pair<IRCErrorCode, std::string>	result =
		clients[i].checkFlood(command_line, pollfds[i].fd);
	IRCErrorCode	code = result.first;
	std::string	warning = result.second;
	if (!warning.empty())
	{
		clients[i].out_buffer += warning;
		pollfds[i].events |= POLLOUT;
	}
	return (code);
}

void	Server::processCommand(int client_fd, const std::string &cmdName,
		const std::vector<std::string> &params)
{
	Client	*client = getClientByFd(client_fd);
	if (!client)
	{
		std::cerr << "Error: Unknown message by a Client: " << client_fd
			<< std::endl;
		return ;
	}
	if (!_commandHandler)
	{
		std::cerr << "Error: CommandHandler is not initialized!" << std::endl;
		return ;
	}
	_commandHandler->handleCommand(cmdName, client, params);
	client->updateActivity();
}

void	Server::handleCommandExtraction(int i)
{
	std::string	command_line;
	std::string	prefix;
	while (Parser::getNextCommand(clients[i].in_buffer, command_line))
	{
		IRCErrorCode	code = handleFlood(i, command_line);
		if (code == IRC_CONTINUE_LOOP)
			continue ;
		if (code == IRC_DISCONNECT)
			return ;
		if (Parser::handlePrefix(command_line, prefix))
		{
			ParsedCommand	parsed;
			Parser::getCommandName(command_line, parsed);
			Parser::parseParameters(command_line, parsed);
			if (parsed.valid)
				processCommand(pollfds[i].fd, parsed.command_name, parsed.parameters);
		}
	}
}

ssize_t	Server::readClientData(int client_fd, char buffer[], size_t b_size)
{
	ssize_t	bytes_read = recv(client_fd, buffer, b_size, 0);
	return (bytes_read);
}

std::string	Server::buildQuitMessage(const Client &client)
{
	std::string	quit_msg = ":" + client._nickname + "!" + client._username + "@" +
		client.getHost() + " QUIT :Client disconected\r\n";
	return (quit_msg);
}

bool	Server::handleReadError(int slot, int client_fd)
{
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return (false);
	std::cerr << "Error reading from client fd=" << client_fd << ": "
		<< strerror(errno) << std::endl;
	closeFd(slot, client_fd);
	return (true);
}

void	Server::handleClientDisconnection(int slot)
{
	int	client_fd = pollfds[slot].fd;
	std::cout << "Client disconected: fd=" << client_fd << std::endl;
	if (!clients[slot]._nickname.empty())
	{
		std::string quit_msg = buildQuitMessage(clients[slot]);
		relayMessage(slot, quit_msg);
	}
	closeFd(slot, client_fd);
}

void	Server::processData(int slot, const std::string &data)
{
	handleBufferAccumulation(slot, data);
	clients[slot].updateActivity();
	handleCommandExtraction(slot);
}

void	Server::handleClientRead(int i)
{
	char	buffer[BUFFER_SIZE + 1];
	std::memset(buffer, 0, sizeof(buffer));
	int	client_fd = pollfds[i].fd;
	ssize_t	bytes = readClientData(client_fd, buffer, BUFFER_SIZE);
	if (bytes == 0)
	{
		handleClientDisconnection(i);
		return ;
	}
	if (bytes < 0)
	{
		if (!handleReadError(i, client_fd))
			return ;
		return ;
	}
	std::string	data(buffer, bytes);
	processData(i, data);
}

int	Server::callPoll()
{
	const int	POLL_TIMEOUT = 500;
	int	value = poll(pollfds, MAX_CLIENTS, POLL_TIMEOUT);
	return (value);
}

IRCErrorCode	Server::handlePollError(int poll_result)
{
	if (poll_result < 0)
	{
		if (errno == EINTR)
			return (IRC_BREAK_LOOP);
		std::cerr << "Error: poll() failed: " << strerror(errno) << std::endl;
		return (IRC_BREAK_LOOP);
	}
	if (poll_result == 0)
		return (IRC_CONTINUE_LOOP);
	return (IRC_SUCCESS);
}

void	Server::processPollEvents()
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd == -1)
			continue ;
		if (pollfds[i].revents == 0)
			continue ;
		handlePollEventForSlot(i);
	}
}

void	Server::handlePollEventForSlot(int slot)
{
	if (pollfds[slot].revents & (POLLHUP | POLLERR | POLLNVAL))
	{
		handlePollErrorEvent(slot);
		return ;
	}
	if (pollfds[slot].revents & POLLIN)
	{
		if (slot == 0)
			handleNewConnection();
		else
			handleClientRead(slot);
	}
	if (slot != 0 && (pollfds[slot].revents & POLLOUT))
		handleClientWrite(slot);
}

void	Server::handlePollErrorEvent(int slot)
{
	if (slot == 0)
	{
		std::cerr << "Warning: Poll error on server socket" << std::endl;
		return ;
	}
	int	client_fd = pollfds[slot].fd;
	if (pollfds[slot].revents & POLLHUP)
		std::cout << "Client hangup (POLLHUP): fd=" << client_fd << std::endl;
	else if (pollfds[slot].revents & POLLERR)
		std::cerr << "Client error (POLLERR): fd=" << client_fd << std::endl;
	else if (pollfds[slot].revents & POLLNVAL)
		std::cerr << "Invalid fd (POLLNVAL): fd=" << client_fd << std::endl;
	closeFd(slot, client_fd);
}

void	Server::closeFd(int i, int fd)
{
	removeClientFromAllChannels(&clients[i]);
	close(fd);
	pollfds[i].fd = -1;
	pollfds[i].events = 0;
	clients[i] = Client();
	return ;
}

int	Server::getClientIndex(Client *client) const
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (&clients[i] == client)
			return (i);
	}
	return (-1);
}

void	Server::relayMessage(int senderIndex, const std::string &msg)
{
	for (int j = 1; j < MAX_CLIENTS; ++j)
	{
		if (pollfds[j].fd != -1 && j != senderIndex)
		{
			clients[j].out_buffer += msg;
			pollfds[j].events |= POLLOUT;
		}
	}
}

Client	*Server::getClientByFd(int fd)
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd == fd && pollfds[i].fd > 0)
			return (&clients[i]);
	}
	return (NULL);
}

Channel	*Server::createChannel(const std::string &name)
{
	if (_channels.find(name) != _channels.end())
		return (_channels[name]);
	try
	{
		Channel *newChannel = new Channel(name, this);
		_channels[name] = newChannel;
		return (newChannel);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error creating channel: " << e.what() << std::endl;
		return (NULL);
	}
}

void	Server::sendPingToAllClients()
{
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd != -1)
		{
			std::ostringstream oss;
			long	timestamp = time(NULL);
			oss << timestamp;
			std::string token = oss.str();
			std::string ping_msg = ":ircserv PING :" + token + "\r\n";
			//sendMessageToClient(&clients[i], ping_msg);
			clients[i]._expected_pong_token = token;
			std::cout << "PING sent to " << clients[i].getNick() << std::endl;
		}
	}
}

int	Server::getServerFd() const
{
	return (this->_server_fd);
}

unsigned int	Server::getPort() const
{
	return (this->_port);
}

const std::string	&Server::getPassword() const
{
	return (this->_password);
}

bool	Server::getServerStatus() const
{
	return (this->_running);
}

struct sockaddr_in	Server::getServerAddr() const
{
	return (this->_server_addr);
}

CommandHandler	*Server::getCommandHandler() const
{
	return (this->_commandHandler);
}

const std::string	Server::getServerName() const
{
	return (this->_serverName);
}

const std::string	Server::getHostIP() const
{
	struct sockaddr_in	addr;
	socklen_t	addr_len = sizeof(addr);
	if (getsockname(_server_fd, (struct sockaddr *)&addr, &addr_len) == 0)
		return (inet_ntoa(addr.sin_addr));
	return ("127.0.0.1");
}

Channel	*Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return (it->second);
	return (NULL);
}

Client	*Server::getClientByNick(const std::string &nick)
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd != -1 && clients[i]._nickname == nick)
			return (&clients[i]);
	}
	return (NULL);
}

void	Server::closeSocketServer()
{
	if (_server_fd != -1)
	{
		close(_server_fd);
		_server_fd = -1;
	}
}

void	Server::closeAllSocketClient()
{
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd != -1)
		{
			close(pollfds[i].fd);\
			pollfds[i].fd = -1;
		}
	}
}

void	Server::flushAllClientBuffers()
{
	bool	pending = true;
	while (pending)
	{
		pending = false;
		for (int i = 1; i < MAX_CLIENTS; ++i)
		{
			if (pollfds[i].fd != -1 && !clients[i].out_buffer.empty())
			{
				pending = true;
				handleClientWrite(i);
			}
		}
	}
}

void	Server::queueMessageForAllClients(std::string msg)
{
	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (pollfds[i].fd != -1)
		{
			clients[i].out_buffer += msg;
		}
	}
}

void	Server::removeClientFromAllChannels(Client *client)
{
	for (std::map<std::string, Channel *>::iterator it = _channels.begin();
			it != _channels.end(); ++it)
		it->second->removeClient(client);
}

void Server::stop()
{
	std::string shutdown_msg = ":ircserv QUIT :ircserv shutting down by admin\r\n";
	queueMessageForAllClients(shutdown_msg);
	flushAllClientBuffers();
	closeAllSocketClient();
	closeSocketServer();
}

Server::~Server()
{
	stop();
	delete _commandHandler;
	for (std::map<std::string, Channel *>::iterator it = _channels.begin();
		it != _channels.end(); ++it)
		delete it->second;
	_channels.clear();
}
