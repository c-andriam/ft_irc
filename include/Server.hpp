/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 18:46:52 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:36 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"
# include "Client.hpp"
# include "ErrorCodes.h"
# include "commands/CommandHandler.hpp"
# include <ctime>
# include <map>
# include <netinet/in.h>
# include <poll.h>
# include <string>
# include <sys/poll.h>
# include <sys/socket.h>
# include <vector>

# define MAX_CLIENTS 1024
# define BUFFER_SIZE 4096
# define MAX_BUFFER_SIZE 8172

class Channel;
class Client;
class CommandHandler;

class Server
{
	public:
		Server(unsigned int port, const std::string &password);
		~Server();

		void				run();
		void				stop();
		int					getServerFd() const;
		unsigned int		getPort() const;
		const std::string	&getPassword() const;
		bool				getServerStatus() const;
		struct sockaddr_in	getServerAddr() const;
		CommandHandler		*getCommandHandler() const;
		const std::string	getServerName() const;
		const std::string	getHostIP() const;
		Client				*getClientByFd(int fd);
		Client				*getClientByNick(const std::string &nick);
		int					getClientIndex(Client *client) const;
		Channel				*getChannel(const std::string &name);
		Channel				*createChannel(const std::string &name);
		void				sendMessageToClient(Client *client, const std::string &msg);
		void				sendErrorMessage(int i, int error_code, const std::string &cmd_name,
				const std::string &msg);
		void				relayMessage(int senderIndex, const std::string &msg);
		void				sendMessageImmediately(Client *client, const std::string &msg);
		void				removeClientFromAllChannels(Client *client);
		void				sendPingToAllClients();
		bool				isValidClientIndex(int index) const;
		bool				isValidFd(int fd) const;
		void				closeFd(int i, int fd);

	private:
		int									_server_fd;
		unsigned int						_port;
		bool								_running;
		time_t								_last_ping_time;
		struct sockaddr_in					_server_addr;
		std::string							_password;
		std::string							_serverName;
		struct pollfd						pollfds[MAX_CLIENTS];
		Client								clients[MAX_CLIENTS];
		std::map<std::string, Channel *>	_channels;
		CommandHandler						*_commandHandler;

		void			initClientsArrays();
		void			initializeCommandHandler();
		void			registerAllCommands();
		void			cleanupCommandHandler();
		void			createServerSocket();
		void			configSocketOptions();
		void			setSocketNonBlocking(int fd);
		void			prepareServerAddress();
		void			bindServerSocket();
		void			listenServerSocket();
		ssize_t			readClientData(int client_fd, char buffer[], size_t b_size);
		void			handleClientDisconnection(int slot);
		std::string		buildQuitMessage(const Client &client);
		bool			handleReadError(int slot, int client_fd);
		void			processData(int slot, const std::string &data);
		int				acceptNewClient(struct sockaddr_in &client_addr);
		std::string		extractClientIP(const struct sockaddr_in &address);
		int				findFreeClientSlot();
		void			initializeNewClient(int slot, int client_fd, const std::string &ip);
		void			rejectConnection(int client_fd, const std::string &reason);
		int				callPoll();
		IRCErrorCode	handlePollError(int poll_result);
		void			processPollEvents();
		void			handlePollEventForSlot(int slot);
		void			handlePollErrorEvent(int slot);
		ssize_t			sendDataToClient(int client_fd, const std::string &data);
		void			updateOutput(std::string &buffer, ssize_t bytes_sent);
		bool			handleSendError(int slot, int client_fd);
		void			initializeServer();
		bool			shouldContinueRunning();
		void			handlePing();
		void			checkClientTimeout();
		bool			shouldBreakLoop(IRCErrorCode code);
		void			initSocket();
		void			initPoll();
		void			closeSocketServer();
		void			closeAllSocketClient();
		IRCErrorCode	handlePoll();
		void			updatePollEvents();
		void			handleNewConnection();
		void			handleClientRead(int i);
		void			handleClientWrite(int i);
		void			handleCommandExtraction(int i);
		void			handleBufferAccumulation(int i, const std::string &data);
		IRCErrorCode	handleFlood(int i, std::string command_line);
		void			handleGroupMsg(int i, int client_fd, std::string msg);
		void			processCommand(int client_fd, const std::string &cmd_name,
                      const std::vector<std::string> &params);
		void			flushAllClientBuffers();
		void			queueMessageForAllClients(std::string msg);
};

#endif
