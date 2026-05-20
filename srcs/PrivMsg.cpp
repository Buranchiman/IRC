/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 13:30:31 by wivallee          #+#    #+#             */
/*   Updated: 2026/05/20 14:56:23 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/PrivMsg.hpp"
#include "../includes/Commande.hpp"
#include <iostream>
#include <unistd.h>

PrivMsg::PrivMsg(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

PrivMsg::~PrivMsg()
{
}

void PrivMsg::execute(Client &client, const std::string &args)
{
	message(client, args);
}

void PrivMsg::message(Client &client, const std::string &msg)
{
    bool    isPrivate = true;
    int     len;
    int     i = 8;
    int     j = 0;
	if (!channels_)
		return ;
    len = msg.size();
	while (i < msg.size())
    {
        if (msg[i] == ' ')
            continue;
        else
        {
            if (msg[i] == '#')
                isPrivate = false;
            int j = 0;
            while (msg[i + j] && msg[i + j] == ' ')
                j++;
            break ;
        }
        i++;
    }
    std::string target = msg.substr(i, j);
    if (!isPrivate && client.getChannel()->getName() != target)
    {
        std::string err404 = ":localhost 404 " + client.getNickName() + " " + target + " :Cannot send to channel" + "\r\n";
        send_all(client.getFdSocket(), err404);
    }
}

	std::string out = ":" + sender.getNickName() + "!" + sender.getUserName() + "@localhost " + msg + "\r\n";
	for (unsigned long i = 0; i < clients_.size() ; i++)
	{
		if (clients_[i] != &sender)
		{
			std::cout << "name of client is" << clients_[i]->getNickName() << std::endl;
			send(clients_[i]->getFdSocket(), out.c_str(), out.size(), MSG_NOSIGNAL);
			//write(clients_[i]->getFdSocket(), out.c_str(), out.size());
		}
	}