/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:12:08 by buranchiman       #+#    #+#             */
/*   Updated: 2026/06/10 14:32:58 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Who.hpp"

Who::Who(std::vector<Client *> &/*clients*/, std::vector<Channel> &channels)
	: channels_(&channels)
{
}

Who::~Who()
{
}

void Who::execute(Client &client, const std::string &args)
{
    std::string target = args;
    std::vector<Client *> clients;
    std::cout << "entered Who" << std::endl;
    for (std::vector<Channel>::iterator it = channels_->begin(); it != channels_->end(); ++it)
    {
        if (it->getName() == target)
        {
            clients = it->getMembers();
            break;
        }
    }
    std::string end =
            ":localhost 315 " + client.getNickName() +
            " " + target + " :End of WHO list.\r\n";
    if (clients.empty())
    {
        send_all(client.getFdSocket(), end);
        return ;
    }
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        std::string userList = 
            ":localhost 352 " + client.getNickName() + " " + target + " " + (*it)->getUserName() + " localhost localhost " + (*it)->getNickName() + " H :0 " + (*it)->getUserName() + "\r\n" ;
        send_all(client.getFdSocket(), userList);
    }
    send_all(client.getFdSocket(), end);
    std::cout << "ended Who" <<std::endl;
}
