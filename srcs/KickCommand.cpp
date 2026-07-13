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

KickCommand::KickCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

KickCommand::~KickCommand()
{
}

void KickCommand::execute(Client &client, const std::string &args)
{
	std::string channel_name, target_nick, reason;
	parseKick(args, channel_name, target_nick, reason);

	if (channel_name.empty() || target_nick.empty())
	{
		send_all(client.getFdSocket(), ":localhost 461 " + client.getNickName() + " KICK :Not enough parameters\r\n");
		return;
	}
	kick(client, channel_name, target_nick, reason);
}

void KickCommand::kick(Client &client, const std::string &channel_name,
					   const std::string &target_nick, const std::string &reason)
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
		send_all(client.getFdSocket(), ":localhost 403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n");
		return;
	}
	if (!channel->findClientByNickname(client.getNickName()))
	{
		send_all(client.getFdSocket(), ":localhost 442 " + client.getNickName() + " " + channel_name + " :You're not on that channel\r\n");
		return;
	}
	if (!channel->isOperator(client))
	{
		send_all(client.getFdSocket(), ":localhost 482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n");
		return;
	}
	Client *target = channel->findClientByNickname(target_nick);
	if (!target)
	{
		send_all(client.getFdSocket(), ":localhost 441 " + client.getNickName() + " " + target_nick + " " + channel_name + " :They aren't on that channel\r\n");
		return;
	}

	std::string kick_msg = ":" + client.getNickName() + "!" + client.getUserName()
		+ "@localhost KICK " + channel_name + " " + target_nick + " :"
		+ (reason.empty() ? client.getNickName() : reason) + "\r\n";
	channel->broadcastToAll(kick_msg);

	channel->leave(*target);

	// suppChannel a un bug (condition inversee), on retire le channel directement
	std::vector<Channel *> &chans = target->getChannels();
	for (std::vector<Channel *>::iterator it = chans.begin(); it != chans.end(); ++it)
	{
		if (*it == channel)
		{
			chans.erase(it);
			break;
		}
	}

	std::cout << "[" << client.getNickName() << "] KICK " << channel_name << " " << target_nick << std::endl;
}
