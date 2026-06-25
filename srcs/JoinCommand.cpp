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

	join(client, channelName, password);
}

void JoinCommand::join(Client &client, const std::string &channel_name, const std::string &key)
{
	if (!channels_)
		return ;
	if (channel_name.empty())
		return ;
	for (std::vector<Channel>::iterator it = channels_->begin(); it != channels_->end(); ++it)
	{
		if (it->getName() == channel_name)
		{
			if (it->isInviteOnly() && !it->isInvited(client))
			{
				std::string msg = "473 " + client.getNickName() + " " + channel_name + " :Cannot join channel (+i)\r\n";
				send_all(client.getFdSocket(), msg);
				return;
			}
			if (it->hasPassword() && !it->checkPassword(key))
			{
				std::string msg = "475 " + client.getNickName() + " " + channel_name + " :Cannot join channel (+k)\r\n";
				send_all(client.getFdSocket(), msg);
				return;
			}
			if (it->isUserLimitReached())
			{
				std::string msg = "471 " + client.getNickName() + " " + channel_name + " :Cannot join channel (+l)\r\n";
				send_all(client.getFdSocket(), msg);
				return;
			}
			// if (current && current != &(*it))
			// 	current->leave(client);
			if ((*it).findClientByNickname(client.getNickName()))
				return ;
			it->join(client);
			it->removeInvite(client);

			// send_all JOIN confirmation
			std::string joinMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost JOIN " + channel_name + "\r\n";
			const std::vector<Client*> &clientsList = it->getMembers();
			for (unsigned long i = 0; i < clientsList.size(); ++i)
			{
				send_all(clientsList[i]->getFdSocket(), joinMsg);
			}

			// send_all topic (331/332)
			std::string topic = it->getTopic();
			if (topic.empty())
			{
				std::string noTopic = ":localhost 331 " + client.getNickName() + " " + channel_name + " :No topic is set\r\n";
				send_all(client.getFdSocket(), noTopic);
			}
			else
			{
				std::string topicMsg = ":localhost 332 " + client.getNickName() + " " + channel_name + " :" + topic + "\r\n";
				send_all(client.getFdSocket(), topicMsg);
			}

			// send_all NAMES list (353)
			std::string namesList = ":localhost 353 " + client.getNickName() + " = " + channel_name + " :";
			const std::vector<Client*> &members = it->getMembers();
			for (unsigned long i = 0; i < members.size(); ++i)
			{
				namesList += members[i]->getNickName();
				if (i < members.size() - 1)
					namesList += " ";
			}
			namesList += "\r\n";
			send_all(client.getFdSocket(), namesList);

			// send_all end of NAMES (366)
			std::string endNames = ":localhost 366 " + client.getNickName() + " " + channel_name + " :End of NAMES list\r\n";
			send_all(client.getFdSocket(), endNames);

			return ;
		}
	}
	std::string msg = "403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
	send_all(client.getFdSocket(), msg);
}
