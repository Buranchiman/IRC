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

#include "../includes/Commande.hpp"
#include "../includes/JoinCommand.hpp"
#include <iostream>
#include <unistd.h>

JoinCommand::JoinCommand(std::vector<Channel *> &channels) : channels_(channels)
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

void JoinCommand::join(Client &client, const std::string &channel_name,
	const std::string &key)
{
	Channel	*channel;
	Channel	*created;

	if (channel_name.empty())
		return ;
	for (std::vector<Channel *>::iterator it = channels_.begin(); it != channels_.end(); ++it)
	{
		channel = *it;
		if (!channel)
			continue ;
		if (channel->getName() == channel_name)
		{
			if (channel->isInviteOnly() && !channel->isInvited(client))
			{
				std::string msg = "473 " + client.getNickName() + " "
					+ channel_name + " :Cannot join channel (+i)\r\n";
				send_all(client.getFdSocket(), msg);
				return ;
			}
			if (channel->hasPassword() && !channel->checkPassword(key))
			{
				std::string msg = "475 " + client.getNickName() + " "
					+ channel_name + " :Cannot join channel (+k)\r\n";
				send_all(client.getFdSocket(), msg);
				return ;
			}
			if (channel->isUserLimitReached())
			{
				std::string msg = "471 " + client.getNickName() + " "
					+ channel_name + " :Cannot join channel (+l)\r\n";
				send_all(client.getFdSocket(), msg);
				return ;
			}
			if (channel->findClientByNickname(client.getNickName()))
				return ;
			channel->join(client);
			channel->removeInvite(client);
			std::string joinMsg = ":" + client.getNickName() + "!"
				+ client.getUserName() + "@localhost JOIN " + channel_name
				+ "\r\n";
			const std::vector<Client *> &clientsList = channel->getMembers();
			for (unsigned long i = 0; i < clientsList.size(); ++i)
				send_all(clientsList[i]->getFdSocket(), joinMsg);
			std::string topic = channel->getTopic();
			if (topic.empty())
			{
				std::string noTopic = ":localhost 331 " + client.getNickName()
					+ " " + channel_name + " :No topic is set\r\n";
				send_all(client.getFdSocket(), noTopic);
			}
			else
			{
				std::string topicMsg = ":localhost 332 " + client.getNickName()
					+ " " + channel_name + " :" + topic + "\r\n";
				send_all(client.getFdSocket(), topicMsg);
			}
			std::string namesList = ":localhost 353 " + client.getNickName()
				+ " = " + channel_name + " :";
			const std::vector<Client *> &members = channel->getMembers();
			for (unsigned long i = 0; i < members.size(); ++i)
			{
				if (channel->isOperator(*members[i]))
					namesList += "@" + members[i]->getNickName();
				else
					namesList += members[i]->getNickName();
				if (i < members.size() - 1)
					namesList += " ";
			}
			namesList += "\r\n";
			send_all(client.getFdSocket(), namesList);
			std::string endNames = ":localhost 366 " + client.getNickName()
				+ " " + channel_name + " :End of NAMES list\r\n";
			send_all(client.getFdSocket(), endNames);
			return ;
		}
	}
	std::string normalized_name = channel_name;
	if (normalized_name.empty() == false && normalized_name[0] != '#')
		normalized_name = std::string("#") + normalized_name;
	created = new Channel(normalized_name, "");
	channels_.push_back(created);
	if (!key.empty())
		created->setPassword(key);
	created->join(client);
	created->addOperator(client);
	created->removeInvite(client);
	std::string joinMsg = ":" + client.getNickName() + "!"
		+ client.getUserName() + "@localhost JOIN " + normalized_name + "\r\n";
	send_all(client.getFdSocket(), joinMsg);
	std::string noTopic = ":localhost 331 " + client.getNickName() + " "
		+ normalized_name + " :No topic is set\r\n";
	send_all(client.getFdSocket(), noTopic);
	std::string namesList = ":localhost 353 " + client.getNickName() + " = "
		+ normalized_name + " :";
	const std::vector<Client *> &membersCreated = created->getMembers();
	for (unsigned long i = 0; i < membersCreated.size(); ++i)
	{
		if (created->isOperator(*membersCreated[i]))
			namesList += "@" + membersCreated[i]->getNickName();
		else
			namesList += membersCreated[i]->getNickName();
		if (i < membersCreated.size() - 1)
			namesList += " ";
	}
	namesList += "\r\n";
	send_all(client.getFdSocket(), namesList);
	std::string endNames = ":localhost 366 " + client.getNickName() + " "
		+ normalized_name + " :End of NAMES list\r\n";
	send_all(client.getFdSocket(), endNames);
}
