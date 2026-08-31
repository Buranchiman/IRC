/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 13:30:31 by wivallee          #+#    #+#             */
/*   Updated: 2026/08/31 15:33:01 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/PrivMsg.hpp"
#include <iostream>
#include <unistd.h>

PrivMsg::PrivMsg(std::vector<Client *> &clients, std::vector<Channel *> &channels)
	: clients_(clients), channels_(channels)
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
	std::string args = msg;
	while (!args.empty() && args[0] == ' ')
		args.erase(0, 1);
	if (args.empty())
		return;
	size_t space = args.find(' ');
	if (space == std::string::npos)
	{
		std::string err = ":localhost 411 " + client.getNickName() + " :No recipient given (PRIVMSG)\r\n";
		send_all(client.getFdSocket(), err);
		return;
	}
	std::string target = args.substr(0, space);
	std::string message;
	size_t colon = args.find(" :");
	if (colon != std::string::npos)
		message = args.substr(colon + 2);
	else if (space + 1 < args.size())
		message = args.substr(space + 1);
	if (target.empty() || message.empty())
	{
		std::string err = ":localhost 412 " + client.getNickName() + " :No text to send\r\n";
		send_all(client.getFdSocket(), err);
		return;
	}
	if (!target.empty() && target[0] == '#')
	{
		Channel *channel = NULL;
		channel = findChannelByName(target, channels_, client);
		if (!channel)
			return ;
		if (!channel->findClientByNickname(client.getNickName()))
		{
			std::string err = ":localhost 404 " + client.getNickName() + " " + target + " :Cannot send to channel\r\n";
			send_all(client.getFdSocket(), err);
			return;
		}
		channel->msgEveryone(client, "PRIVMSG " + target + " :" + message);
		return;
	}
	Client *recipient = findClientByNickname(clients_, target);
	if (!recipient)
	{
		std::string err = ":localhost 401 " + client.getNickName() + " " + target + " :No such nick/channel\r\n";
		send_all(client.getFdSocket(), err);
		return;
	}
	std::string out = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
	send_all(recipient->getFdSocket(), out);
}
