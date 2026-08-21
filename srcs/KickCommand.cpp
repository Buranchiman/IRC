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
		kick(client, target, reason);
	}
}

void KickCommand::kick(Client &client, const std::string &target_name, const std::string &reason)
{
	bool found_shared = false;
	for (size_t i = 0; i < channels_->size(); ++i)
	{
		Channel &ch = (*channels_)[i];
		Client *t = ch.findClientByNickname(target_name);
		if (t)
		{
			// target is in this channel
			Client *c = ch.findClientByNickname(client.getNickName());
			if (c)
			{
				found_shared = true;
				if (!ch.isOperator(client))
				{
					std::string msg = "482 " + client.getNickName() + " " + ch.getName() + " :You're not channel operator\r\n";
					write(client.getFdSocket(), msg.c_str(), msg.size());
					return;
				}
				kick(client, ch.getName(), target_name, reason);
				return;
			}
		}
	}
	if (!found_shared)
	{
		std::string msg = "441 " + client.getNickName() + " " + target_name + " :They aren't on that channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
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
	// si le channel n'existe pas, on envoie une erreur au client
	if (!channel)
	{
		std::string msg = "403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}
	// si le client n'est pas opérateur du channel, on envoie une erreur au client
	if (!channel->isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}
	Client *target = channel->findClientByNickname(target_name);
	
	//si le client cible n'est pas dans le channel, on envoie une erreur au client par rapport au nickname
	if (!target)
	{
		std::string msg = "401 " + client.getNickName() + " " + target_name + " :No such nick/channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	std::string kick_msg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost KICK " + channel_name + " " + target_name;
	if (!reason.empty())
		kick_msg += " :" + reason;
	kick_msg += "\r\n";
	channel->broadcastToAll(kick_msg);
	channel->leave(*target);
	target->suppChannel(channel);
	std::string msg = "You have been kicked from " + channel_name + "\r\n";
	write(target->getFdSocket(), msg.c_str(), msg.size());
	msg = "Please join a channel using: JOIN #channelname\r\n";
	write(target->getFdSocket(), msg.c_str(), msg.size());

	// action de kick exécutée : le client a été supprimé du channel et notifié
}
