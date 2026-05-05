/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TopicCommand.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/TopicCommand.hpp"
#include "../includes/Commande.hpp"
#include <iostream>
#include <unistd.h>

TopicCommand::TopicCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

TopicCommand::~TopicCommand()
{
}

void TopicCommand::execute(Client &client, const std::string &args)
{
	std::string topic;
	parseTopic(args, topic);
	
	std::cout << "[" << client.getNickName() << "] TOPIC :" << topic << std::endl;
	TopicCommand::topic(client, topic);
}

void TopicCommand::topic(Client &client, const std::string &new_topic)
{
	Channel *channel = client.getChannel();
	if (!channel)
	{
		std::string msg = "403 " + client.getNickName() + " * :You are not in a channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}
	topic(client, channel->getName(), new_topic);
}

void TopicCommand::topic(Client &client, const std::string &channel_name, const std::string &new_topic)
{
	Channel *channel = NULL;
	for (size_t i = 0; i < channels_->size(); ++i)
	{
		if ((*channels_)[i].getName() == channel_name)
		{
			channel = &(*channels_)[i];
			break;
		}
	}

	if (!channel)
	{
		std::string msg = "403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	if (new_topic.empty())
	{
		channel->sendTopic(client);
	}
	else
	{
		channel->setTopic(new_topic, client);
	}
}
