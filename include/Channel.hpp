/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 23:54:51 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:16 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <map>
# include <string>
# include <vector>
# include "../include/Client.hpp"

class	Channel
{
	private:
		std::string					_name;
		std::string					_topic;
		std::map<char, std::string>	_modes;
		std::vector<Client*>		_clients;
		std::vector<Client*>		_operators;
		std::vector<Client*>		_invited;
        Server*                     _server;
		int							_user_limit;
	
	public:
		Channel( const std::string& name, Server* server );
		~Channel();

		const std::string&			getName() const;
		std::string					getTopic() const;
		std::vector<Client*>		getClients() const;

		bool						hasMode( char mode ) const;
		const std::string&			getModeParam( char mode ) const;
		void						setMode( char mode, const std::string& param );
		void						unsetMode( char mode );
		bool						isValidMode( char mode ) const;

		bool						hasKey() const;
		std::string					getKey() const;
		bool						isInviteOnly() const;
		bool						isInvited(Client* client) const;
		bool						hasUserLimit() const;
		int							getUserLimit() const;
		bool						isFull() const;
		bool						hasClient(Client* client) const;

		void						addInvite(Client* client);
		void						removeInvite(Client* client);
		void						setTopic( const std::string& topic );
		bool						addClient( Client* client );
		bool						removeClient( Client* client );
		bool						addOperator( Client* client );
		bool						removeOperator( Client* client );
		bool						isOperator( Client* client ) const;
		bool						isClientInChannel( Client* client ) const;
		bool						isValidChannelName( const std::string& name );
		void						broadcastMessage( const std::string& message, Client* sender = NULL );
		std::string					getUserList() const;

};
