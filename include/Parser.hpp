/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: candriam <candriam@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 06:50:20 by candriam          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:30 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>

struct ParsedCommand
{
	std::string					command_name;
	std::vector<std::string>	parameters;
	bool						valid;
};

class Parser
{
	public:
		static bool	getNextCommand( std::string& in_buffer, std::string& command );
		static bool	hasPrefix( std::string &extracted_cmd );
		static bool	handlePrefix( std::string& extracted_cmd, std::string& prefix );
		static bool	getCommandName( std::string& extracted_cmd, ParsedCommand& parsed );
		static bool	parseParameters( std::string& extracted_cmd, ParsedCommand& parsed );

	private:
		static bool	validateCommandString( const std::string& cmd );
		static bool	parseParameterList( std::string& cmd, std::vector<std::string>& parameters );
		static bool	validateParameterCount( const std::string& cmd_name, const std::vector<std::string>& parameters );
};

#endif
