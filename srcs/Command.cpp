/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 16:19:51 by wivallee          #+#    #+#             */
/*   Updated: 2026/08/23 16:49:45 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Command.hpp"

Channel	*Command::findChannelByName(std::string name, std::vector<Channel *> channels_, Client &client)
{
    for (size_t i = 0; i < channels_.size(); ++i)
	{
		if (channels_[i] && channels_[i]->getName() == name)
		{
			return(channels_[i]);
		}
	}
	std::string msg = ":localhost 403 " + client.getNickName() + " " + name + " :No such channel\r\n";
    std::cout << "[DEBUG] 403 is " + msg;
	send_all(client.getFdSocket(), msg);
    return (NULL);
}