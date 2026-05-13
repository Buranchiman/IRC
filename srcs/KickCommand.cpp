/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/KickCommand.hpp"
#include "../includes/Commande.hpp"
#include <iostream>
#include <unistd.h>

KickCommand::KickCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

KickCommand::~KickCommand()
{
}

void KickCommand::execute(Client &client, const std::string &args)
{
	std::string channel, target, reason;
	parseKick(args, channel, target, reason);

	if (!channel.empty() && !target.empty())
	{
		std::cout << "[" << client.getNickName() << "] KICK " << channel << " " << target << std::endl;
		kick(client, channel, target, reason);
	}
}

void KickCommand::kick(Client &client, const std::string &channel_name, const std::string &target_name, const std::string &reason)
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
	if (!channel->isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}
	Client *target = channel->findClientByNickname(target_name);
	if (!target)
	{
		std::string msg = "401 " + client.getNickName() + " " + target_name + " :No such nick/channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	std::string kick_msg = ":" + client.getNickName() + " KICK " + channel_name + " " + target_name;
	if (!reason.empty())
		kick_msg += " :" + reason;
	kick_msg += "\r\n";
	channel->broadcastToAll(kick_msg);
	channel->leave(*target);
	target->setChannel(NULL);
	std::string msg = "You have been kicked from " + channel_name + "\r\n";
	write(target->getFdSocket(), msg.c_str(), msg.size());
	msg = "Please join a channel using: JOIN #channelname\r\n";
	write(target->getFdSocket(), msg.c_str(), msg.size());

	std::cout << "[" << client.getNickName() << "] KICK " << channel_name << " " << target_name << std::endl;
}
