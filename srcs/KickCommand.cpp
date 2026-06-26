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
#include "../includes/Command.hpp"
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
	std::string channel_name, target_nick, reason;
	parseKick(args, channel_name, target_nick, reason);

	if (channel_name.empty() || target_nick.empty())
	{
		std::string msg = ":localhost 461 " + client.getNickName() + " KICK :Not enough parameters\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}
	kick(client, channel_name, target_nick, reason);
}

void KickCommand::kick(Client &client, const std::string &channel_name,
					   const std::string &target_nick, const std::string &reason)
{
	// Trouver le channel
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
		std::string msg = ":localhost 403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}

	// Verifier que le kicker est dans le channel
	if (!channel->findClientByNickname(client.getNickName()))
	{
		std::string msg = ":localhost 442 " + client.getNickName() + " " + channel_name + " :You're not on that channel\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}

	// Verifier que le kicker est operateur
	if (!channel->isOperator(client))
	{
		std::string msg = ":localhost 482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}

	// Trouver la cible dans le channel
	Client *target = channel->findClientByNickname(target_nick);
	if (!target)
	{
		std::string msg = ":localhost 441 " + client.getNickName() + " " + target_nick + " " + channel_name + " :They aren't on that channel\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}

	// Envoyer le message KICK a tous les membres (y compris la cible) avant de la retirer
	std::string kick_msg = ":" + client.getNickName() + "!" + client.getUserName()
		+ "@localhost KICK " + channel_name + " " + target_nick + " :";
	kick_msg += reason.empty() ? client.getNickName() : reason;
	kick_msg += "\r\n";
	channel->broadcastToAll(kick_msg);

	// Retirer la cible du channel
	channel->leave(*target);
	target->suppChannel(channel);

	std::cout << "[" << client.getNickName() << "] KICK " << channel_name << " " << target_nick << std::endl;
}
