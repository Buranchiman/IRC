/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InviteCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/InviteCommand.hpp"
#include "../includes/Commande.hpp"
#include <iostream>
#include <unistd.h>

InviteCommand::InviteCommand(std::vector<Channel> &channels, std::vector<Client *> &clients)
	: channels_(&channels), clients_(clients)
{
}

InviteCommand::~InviteCommand()
{
}
//parse les arguments
void InviteCommand::execute(Client &client, const std::string &args)
{
	std::string target_nick, channel_name;
	parseInvite(args, target_nick, channel_name);

	if (target_nick.empty())
	{
		std::string msg = "461 " + client.getNickName() + " INVITE :Not enough parameters\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	if (channel_name.empty())
	{
		Channel *current = client.getChannel();
		if (!current)
		{
			std::string msg = "403 " + client.getNickName() + " * :You are not in a channel\r\n";
			write(client.getFdSocket(), msg.c_str(), msg.size());
			return;
		}
		channel_name = current->getName();
	}

	std::cout << "[" << client.getNickName() << "] INVITE " << target_nick << " " << channel_name << std::endl;
	invite(client, channel_name, target_nick);
}

void InviteCommand::invite(Client &client, const std::string &channel_name, const std::string &target_name)
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

	Client *target = NULL;
	for (size_t i = 0; i < clients_.size(); ++i)
	{
		if (clients_[i]->getNickName() == target_name)
		{
			target = clients_[i];
			break;
		}
	}

	if (!target)
	{
		std::string error_msg = "401 " + client.getNickName() + " " + target_name + " :No such nick/channel\r\n";
		write(client.getFdSocket(), error_msg.c_str(), error_msg.size());
		return;
	}

	channel->addInvite(*target);

	std::string inviteNotice = ":" + client.getNickName() + "!" + client.getUserName()
		+ "@localhost INVITE " + target_name + " :" + channel_name + "\r\n";
	write(target->getFdSocket(), inviteNotice.c_str(), inviteNotice.size());

	std::string msg = "341 " + client.getNickName() + " " + target_name + " " + channel_name + "\r\n";
	write(client.getFdSocket(), msg.c_str(), msg.size());

	std::cout << "[INVITE] " << client.getNickName() << " invited " << target_name << " to " << channel_name << std::endl;
}
