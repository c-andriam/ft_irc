/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 00:32:59 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 17:37:53 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

Channel::Channel( const std::string& name, Server* server ) : 
	_name(name),
	_topic(""),
	_server(server),
	_user_limit(0)
{
	if (!isValidChannelName(name))
		throw std::runtime_error("Invalid channel name");
}

bool	Channel::isValidChannelName( const std::string& name )
{
	if (name.empty() || name.size() > 50)
		return (false);
	if (name[0] != '#' && name[0] != '&')
		return (false);
	for (std::size_t i = 0; i < name.size(); ++i)
	{
		char	c = name[i];
		if (c == ' ' || c == ',')
			return (false);
		if (c < 32)
			return (false);
		if (i == 1 && (c == ':' || c == '!' || c == '@'))
			return (false);
	}
	return (true);
}

const std::string&	Channel::getName() const
{
	return (this->_name);
}

const std::string&	Channel::getTopic() const
{
	return (this->_topic);
}

const std::vector<Client*>&	Channel::getClients() const
{
	return (_clients);
}

bool	Channel::isValidMode( char mode ) const
{
	if (mode == 'i' || mode == 'k' || mode == 't' || mode == 'l' || mode == 'o')
		return (true);
	return (false);
}

bool	Channel::hasMode( char mode ) const
{
	return (_modes.find(mode) != _modes.end());
}

const std::string&	Channel::getModeParam( char mode ) const
{
	static const std::string	no_param = "";
	std::map<char, std::string>::const_iterator it = _modes.find(mode);
	if (it != _modes.end())
		return (it->second);
	return (no_param);
}

bool	Channel::hasKey() const
{
	return (hasMode('k'));
}

const std::string&	Channel::getKey() const
{
	return (getModeParam('k'));
}

bool	Channel::isInviteOnly() const
{
	return (hasMode('i'));
}

bool	Channel::isInvited(Client* client) const
{
	return (std::find(_invited.begin(), _invited.end(), client) != _invited.end());
}

void	Channel::addInvite(Client* client)
{
	if (!isInvited(client))
		_invited.push_back(client);
}

void	Channel::removeInvite(Client* client)
{
	std::vector<Client*>::iterator	it = std::find(_invited.begin(), _invited.end(), client);
	if (it != _invited.end())
		_invited.erase(it);
}

bool	Channel::hasUserLimit() const
{
	return (hasMode('l'));
}

int	Channel::getUserLimit() const
{
	if (!hasUserLimit())
		return (0);
	std::string	lim = getModeParam('l');
	if (lim.empty())
		return (0);
	return (std::atoi(lim.c_str()));
}

bool	Channel::isFull() const
{
	if (!hasUserLimit())
		return (false);
	int	lim = getUserLimit();
	return (lim > 0 && static_cast<int>(_clients.size()) >= lim);
}

void	Channel::setMode( char mode, const std::string& param )
{
	_modes[mode] = param;
}

void	Channel::unsetMode( char mode )
{
	_modes.erase(mode);
}

bool	Channel::addClient( Client* client )
{
	if (!isClientInChannel(client))
	{
		bool	is_first = _clients.empty();
		_clients.push_back(client);
		if (is_first)
			addOperator(client);
		return (true);
	}
	return (false);
}

bool	Channel::removeClient( Client* client )
{
	if (isClientInChannel(client))
	{
		std::vector<Client*>::iterator it = std::remove(_clients.begin(), _clients.end(), client);
		_clients.erase(it, _clients.end());
		removeOperator(client);
		return (true);
	}
	return (false);
}

bool	Channel::addOperator( Client* client )
{
	if (!isOperator(client))
	{
		_operators.push_back(client);
		return (true);
	}
	return (false);
}

bool	Channel::removeOperator( Client* client )
{
	if (isOperator(client))
	{
		std::vector<Client*>::iterator	it = std::remove(_operators.begin(), _operators.end(), client);
		_operators.erase(it, _operators.end());
		return (true);
	}
	return (false);
}

bool	Channel::isOperator( Client* client ) const
{
	return (std::find(_operators.begin(), _operators.end(), client) != _operators.end());
}

bool	Channel::isClientInChannel( Client* client ) const
{
	for (std::vector<Client*>::const_iterator	cit = _clients.begin(); cit != _clients.end(); ++cit)
	{
		if (*cit == client)
			return (true);
	}
	return (false);
}

void	Channel::setTopic( const std::string& topic )
{
	_topic = topic;
}

void	Channel::broadcastMessage( const std::string& message, Client* sender )
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (sender == NULL || *it != sender)
		{
			_server->sendMessageToClient(*it, message);
		}
	}
}

bool	Channel::hasClient(Client* client) const
{
	return (std::find(_clients.begin(), _clients.end(), client) != _clients.end());
}

std::string	Channel::getUserList() const
{
	std::string list;
	for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (!list.empty())
			list += " ";
		if (isOperator(*it))
			list += "@";
		list += (*it)->getNick();
	}
	return list;
}

Channel::~Channel()
{
}
