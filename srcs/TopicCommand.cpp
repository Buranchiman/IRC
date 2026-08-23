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

TopicCommand::TopicCommand(std::vector<Channel *> &channels)
	: channels_(channels)
{
}

TopicCommand::~TopicCommand()
{
}

void TopicCommand::execute(Client &client, const std::string &args)
{
    std::string channel_name;
    std::string topic;

    parseTopic(args, channel_name, topic);

    if (channel_name.empty())
    {
        std::string msg = "461 " + client.getNickName() + " TOPIC :Not enough parameters\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    TopicCommand::topic(client, channel_name, topic);
}

void TopicCommand::topic(Client &client, const std::string &new_topic)
{
    std::vector<Channel *> &chlist = client.getChannels();
    if (chlist.empty())
    {
        std::string msg = "403 " + client.getNickName() + " * :You are not in a channel\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }
    if (chlist.size() > 1)
    {
        std::string msg = "461 " + client.getNickName() + " TOPIC :Not enough parameters (ambiguous channel)\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }
    topic(client, chlist[0]->getName(), new_topic);
}

void TopicCommand::topic(Client &client, const std::string &channel_name, const std::string &new_topic)
{
    Channel *channel = NULL;
    channel = findChannelByName(channel_name, channels_, client);
    if (!channel)
        return ;
    if (!channel->findClientByNickname(client.getNickName()))
    {
        std::string msg = ":localhost 442 " + client.getNickName() + " " + channel_name + " :You're not on that channel\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    if (new_topic.empty())
    {
        channel->sendTopic(client);
        return;
    }

    if (channel->isTopicRestricted() && !channel->isOperator(client))
    {
        std::string msg = ":localhost 482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }
    channel->setTopic(new_topic, client);
}
