/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 07:00:10 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 17:44:19 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <iostream>
#include <vector>

bool	Parser::getNextCommand( std::string& in_buffer, std::string& command )
{
	std::size_t	cmd_pos;
	if (in_buffer.empty())
		return (false);
	do
	{
		cmd_pos = in_buffer.find("\r\n");
		if (cmd_pos == std::string::npos)
			return (false);
		if (cmd_pos == 0)
			in_buffer.erase(0, cmd_pos + 2);
	}
	while (cmd_pos == 0);
	command = in_buffer.substr(0, cmd_pos);
	if (command.size() > 510)
	{
		std::cerr << "Command too long, ignoring" << std::endl;
		in_buffer.erase(0, cmd_pos + 2);
		return (false);
	}
	in_buffer.erase(0, cmd_pos + 2);
	return (true);
}

bool	Parser::hasPrefix( std::string& extracted_cmd )
{
	if (!extracted_cmd.empty() && extracted_cmd[0] == ':')
		return (true);
	return (false);
}

bool	Parser::handlePrefix( std::string& extracted_cmd, std::string& prefix )
{
	if (!hasPrefix(extracted_cmd))
		return (true);
	std::size_t	sep_pos = extracted_cmd.find(' ');
	if (sep_pos == std::string::npos || sep_pos == 1)
	{
		std::cerr << "Invalid prefix." << std::endl;
		return (false);
	}
	prefix = extracted_cmd.substr(1, sep_pos - 1);
	extracted_cmd.erase(0, sep_pos + 1);
	return (true);
}

bool	Parser::getCommandName( std::string& extracted_cmd, ParsedCommand& parsed )
{
	if (extracted_cmd.empty() || extracted_cmd[0] == ' ')
	{
		std::cerr << "Invalid command: no command name" << std::endl;
		parsed.valid = false;
		return (false);
	}
	std::size_t	sep_pos = extracted_cmd.find(' ');
	if (sep_pos == std::string::npos)
	{
		parsed.command_name = extracted_cmd;
		extracted_cmd.clear();
	}
	else
	{
		parsed.command_name = extracted_cmd.substr(0, sep_pos);
		extracted_cmd.erase(0, sep_pos + 1);
	}
	std::transform(parsed.command_name.begin(), parsed.command_name.end(), parsed.command_name.begin(), ::toupper);
	parsed.valid = true;
	return (true);
}

bool	Parser::validateCommandString( const std::string& cmd )
{
	if (cmd.find('\t') != std::string::npos)
	{
		std::cerr << "Invalid character: tab found" << std::endl;
		return (false);
	}
	for (size_t i = 0; i < cmd.size(); ++i)
	{
		unsigned char	c = cmd[i];
		if (c < 32 && c != 1)
		{
			std::cerr << "Invalid character: control or non-printable" << std::endl;
			return (false);
		}
	}
	return (true);
}

bool	Parser::parseParameterList( std::string& cmd, std::vector<std::string>& parameters )
{
	std::size_t	pos = 0;
	while (pos < cmd.size())
	{
		while (pos < cmd.size() && cmd[pos] == ' ')
			pos++;
		if (pos >= cmd.size())
			break ;
		if (cmd[pos] == ':')
		{
			parameters.push_back(cmd.substr(pos + 1));
			break ;
		}
		std::size_t	end = cmd.find(' ', pos);
		if (end == std::string::npos)
		{
			parameters.push_back(cmd.substr(pos));
			break ;
		}
		parameters.push_back(cmd.substr(pos, end - pos));
		pos = end;
	}
	if (parameters.size() > 15)
	{
		std::cerr << "Too many parameters: max 15" << std::endl;
		return (false);
	}
	return (true);
}

bool	Parser::parseParameters( std::string& extracted_cmd, ParsedCommand& parsed )
{
	parsed.parameters.clear();
	if (!validateCommandString(extracted_cmd))
	{
		parsed.valid = false;
		return (false);
	}
	if (!parseParameterList(extracted_cmd, parsed.parameters))
	{
		parsed.valid = false;
		return (false);
	}
	parsed.valid = true;
	return (true);
}
