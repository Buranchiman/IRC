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
	std::string target, reason;
	parseKick(args, target, reason);

	if (!target.empty())
	{
		std::cout << "[" << client.getNickName() << "] KICK " << target << std::endl;
		kick(client, target, reason);
	}
}

void KickCommand::kick(Client &client, const std::string &target_name, const std::string &reason)
{
	std::vector<Channel *> channels  = client.getChannels();
	std::vector<Channel *> channels_serv = KickCommand.
	for(unsigned int i = 0; channels.size() < i; i++)
	{
		if(channels[i]->getName() == target_name)
		{
			//sup client
			client.suppChannel(channels[i]);
			//sup serv
			
		}
	}

	// if (channels.empty())
	// {
	// 	std::string msg = "403 " + client.getNickName() + " * :You are not in a channel\r\n";
	// 	write(client.getFdSocket(), msg.c_str(), msg.size());
	// 	return;
	// }
	// kick(client, target_name, reason);
}

// void KickCommand::kick(Client &client, std::vector<Channel *> channels, const std::string &target_name, const std::string &reason)
// {
	

// 	Channel *channel = NULL;
// 	for (size_t i = 0; i < channels_->size(); ++i)
// 	{
// 		if ((*channels_)[i].getName() == channel_name)
// 		{
// 			channel = &(*channels_)[i];
// 			break;
// 		}
// 	}
// 	if (!channel)
// 	{
// 		std::string msg = "403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
// 		write(client.getFdSocket(), msg.c_str(), msg.size());
// 		return;
// 	}
// 	if (!channel->isOperator(client))
// 	{
// 		std::string msg = "482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
// 		write(client.getFdSocket(), msg.c_str(), msg.size());
// 		return;
// 	}
// 	Client *target = channel->findClientByNickname(target_name);
// 	if (!target)
// 	{
// 		std::string msg = "401 " + client.getNickName() + " " + target_name + " :No such nick/channel\r\n";
// 		write(client.getFdSocket(), msg.c_str(), msg.size());
// 		return;
// 	}

// 	std::string kick_msg = client.getNickName() + " kicked " + target_name;
// 	if (!reason.empty())
// 		kick_msg += " (" + reason + ")";
// 	kick_msg += "\r\n";
// 	channel->broadcastToAll(kick_msg);
// 	channel->leave(*target);
// 	target->suppChannel(NULL);
// 	std::string msg = "You have been kicked from " + channel_name + "\r\n";
// 	write(target->getFdSocket(), msg.c_str(), msg.size());
// 	msg = "Please join a channel using: JOIN #channelname\r\n";
// 	write(target->getFdSocket(), msg.c_str(), msg.size());

// 	std::cout << "[" << client.getNickName() << "] KICK #" << channel_name << " " << target_name << std::endl;
// }
