/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 00:41:38 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:04:33 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>
# include <vector>
# include "Client.hpp"

class	Reply
{
	private:
		struct	ReplyFormat
		{
			int			code;
			std::string	format;
		};

		static std::vector<ReplyFormat>	_reply_formats;
		static bool						_initialized;

	public:
		~Reply();

		static void			initReplyFormats();
		static std::string	getReplyFormat( int code );
		static void			sendReply( Client* client, int code, const std::string& msg );
};

#endif
