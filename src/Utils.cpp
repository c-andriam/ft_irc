/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tambinin <tambinin@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 23:42:51 by tambinin          #+#    #+#             */
/*   Updated: 2025/10/26 16:06:02 by candriam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>

std::string intToString(int code) {
  std::stringstream ss;
  ss << code;
  if (code == 001 || code == 002 || code == 003 || code == 004)
    return ("00" + ss.str());
  return (ss.str());
}
