/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 14:48:45 by luda-cun          #+#    #+#             */
/*   Updated: 2026/08/23 16:43:42 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/Commande.hpp"
#include "../includes/KickCommand.hpp"
#include <unistd.h>

KickCommand::KickCommand(std::vector<Channel *> &channels) : channels_(channels)
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

void KickCommand::kick(Client &client, const std::string &target_name,
	const std::string &reason)
{
	bool	found_shared;
	Client	*t;
	Client	*c;

	found_shared = false;
	for (size_t i = 0; i < channels_.size(); ++i)
	{
		if (!channels_[i])
			continue ;
		Channel &ch = *channels_[i];
		t = ch.findClientByNickname(target_name);
		if (t)
		{
			c = ch.findClientByNickname(client.getNickName());
			if (c)
			{
				found_shared = true;
				if (!ch.isOperator(client))
				{
					std::string msg = "482 " + client.getNickName() + " "
						+ ch.getName() + " :You're not channel operator\r\n";
					send_all(client.getFdSocket(), msg.c_str());
					return ;
				}
				kick(client, ch.getName(), target_name, reason);
				return ;
			}
		}
	}
	if (!found_shared)
	{
		std::string msg = "441 " + client.getNickName() + " " + target_name
			+ " :They aren't on that channel\r\n";
		send_all(client.getFdSocket(), msg.c_str());
	}
}

void KickCommand::kick(Client &client, const std::string &channel_name,
	const std::string &target_name, const std::string &reason)
{
	Channel	*channel;
	Client	*target;

	channel = NULL;
	channel = findChannelByName(channel_name, channels_, client);
	if (!channel)
		return ;
	if (!channel->isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + channel_name
			+ " :You're not channel operator\r\n";
		send_all(client.getFdSocket(), msg.c_str());
		return ;
	}
	target = channel->findClientByNickname(target_name);
	if (!target)
	{
		std::string msg = "401 " + client.getNickName() + " " + target_name
			+ " :No such nick/channel\r\n";
		send_all(client.getFdSocket(), msg.c_str());
		return ;
	}
	std::string kick_msg = ":" + client.getNickName() + "!"
		+ client.getUserName() + "@localhost KICK " + channel_name + " "
		+ target_name;
	if (!reason.empty())
		kick_msg += " :" + reason;
	kick_msg += "\r\n";
	channel->broadcastToAll(kick_msg);
	channel->leave(*target);
	target->suppChannel(channel);
	std::string msg = "You have been kicked from " + channel_name + "\r\n";
	send_all(target->getFdSocket(), msg.c_str());
	msg = "Please join a channel using: JOIN #channelname\r\n";
	send_all(target->getFdSocket(), msg.c_str());
}
