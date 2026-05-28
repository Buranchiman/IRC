/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoIs.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:12:08 by buranchiman       #+#    #+#             */
/*   Updated: 2026/05/28 10:17:40 by buranchiman      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WhoIs.hpp"

WhoIs::WhoIs(std::vector<Client *> &clients, std::vector<Channel> &channels)
	: clients_(&clients), channels_(&channels)
{
}

WhoIs::~WhoIs()
{
}

void WhoIs::execute(Client &client, const std::string &args)
{
    std::string target = args;

    Client *found = findClientByNickname(*clients_, target);

	std::cout << "entered whois" << std::endl;
    if (!found)
    {
        std::string err =
            ":localhost 401 " + client.getNickName() +
            " " + target + " :No such nick/channel\r\n";

        send_all(client.getFdSocket(), err);

        std::string end =
            ":localhost 318 " + client.getNickName() +
            " " + target + " :End of /WHOIS list.\r\n";

        send_all(client.getFdSocket(), end);
        return;
    }

    std::string rpl311 =
        ":localhost 311 " +
        client.getNickName() + " " +
        found->getNickName() + " " +
        found->getUserName() + " localhost * :" +
        found->getUserName() + "\r\n";

    send_all(client.getFdSocket(), rpl311);

    std::string rpl312 =
    ":localhost 312 " +
    client.getNickName() + " " +
    found->getNickName() +
    " localhost :Local IRC Server\r\n";
    send_all(client.getFdSocket(), rpl312);

    std::string rpl318 =
        ":localhost 318 " +
        client.getNickName() + " " +
        found->getNickName() +
        " :End of /WHOIS list.\r\n";

    send_all(client.getFdSocket(), rpl318);
    std::cout << "ended whois" <<std::endl;
}
