/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JoinCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/JoinCommand.hpp"
#include "../includes/Commande.hpp"
#include <iostream>
#include <unistd.h>

JoinCommand::JoinCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

JoinCommand::~JoinCommand()
{
}

void JoinCommand::execute(Client &client, const std::string &args)
{
	std::string channelName, password;
	parseJoin(args, channelName, password);
	
	std::cout << "[" << client.getNickName() << "] JOIN #" << channelName << std::endl;
	join(client, channelName, password);
}

void JoinCommand::join(Client &client, const std::string &channel_name, const std::string &key)
{
	if (!channels_)
		return ;
	if (channel_name.empty())
		return ;
	std::cout << "[DEBUG] join() channel_name='" << channel_name << "'" << std::endl;
	for (std::vector<Channel>::iterator it = channels_->begin(); it != channels_->end(); ++it)
	{
		std::cout << "[DEBUG] compare with channel='" << it->getName() << "'" << std::endl;
		if (it->getName() == channel_name)
		{
			if (it->isInviteOnly() && !it->isInvited(client))
			{
				std::string msg = "473 " + client.getNickName() + " " + channel_name + " :Cannot join channel (+i)\r\n";
				write(client.getFdSocket(), msg.c_str(), msg.size());
				return;
			}
			if (it->hasPassword() && !it->checkPassword(key))
			{
				std::string msg = "475 " + client.getNickName() + " " + channel_name + " :Cannot join channel (+k)\r\n";
				write(client.getFdSocket(), msg.c_str(), msg.size());
				return;
			}
			if (it->isUserLimitReached())
			{
				std::string msg = "471 " + client.getNickName() + " " + channel_name + " :Cannot join channel (+l)\r\n";
				write(client.getFdSocket(), msg.c_str(), msg.size());
				return;
			}
			Channel *current = client.getChannel();
			if (current && current != &(*it))
				current->leave(client);
			it->join(client);
			it->removeInvite(client);
			std::cout << "[JOIN] " << client.getUserName() << " -> " << it->getName() << std::endl;
			std::string topic = it->getTopic();
			if (topic.empty())
				topic = "(no topic)";
			std::string msg = "TOPIC " + it->getName() + " : " + topic + "\n";
			write(client.getFdSocket(), msg.c_str(), msg.size());
			return ;
		}
	}
	std::string msg = "403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
	write(client.getFdSocket(), msg.c_str(), msg.size());
}
