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

#include "../includes/Commande.hpp"
#include "../includes/InviteCommand.hpp"
#include <iostream>
#include <unistd.h>

InviteCommand::InviteCommand(std::vector<Channel *> &channels,
	std::vector<Client *> &clients) : channels_(channels), clients_(clients)
{
}

InviteCommand::~InviteCommand()
{
}

void InviteCommand::execute(Client &client, const std::string &args)
{
	size_t	sp;
	size_t	pos;

	std::string trimmed = args;
	while (!trimmed.empty() && trimmed[0] == ' ')
		trimmed.erase(0, 1);
	while (!trimmed.empty() && trimmed[trimmed.size() - 1] == ' ')
		trimmed.erase(trimmed.size() - 1, 1);
	if (trimmed.empty())
		return ;
	sp = trimmed.find(' ');
	if (sp == std::string::npos)
	{
		invite(client, trimmed);
	}
	else
	{
		std::string target = trimmed.substr(0, sp);
		pos = sp + 1;
		while (pos < trimmed.size() && trimmed[pos] == ' ')
			pos++;
		std::string channel = (pos < trimmed.size()) ? trimmed.substr(pos) : "";
		invite(client, channel, target);
	}
}

void InviteCommand::invite(Client &client, const std::string &target_name)
{
	std::vector<Channel *> &chlist = client.getChannels();
	if (chlist.empty())
	{
		std::string msg = "403 " + client.getNickName() + "* :You are not in a channel\r\n";
		send_all(client.getFdSocket(), msg.c_str());
		return ;
	}
	if (chlist.size() > 1)
	{
		std::string msg = "461 " + client.getNickName()
			+ " INVITE :Not enough parameters (ambiguous channel)\r\n";
		send_all(client.getFdSocket(), msg.c_str());
		return ;
	}
	invite(client, chlist[0]->getName(), target_name);
}

void InviteCommand::invite(Client &client, const std::string &channel_name,
	const std::string &target_name)
{
	Channel	*channel;
	Client	*target;

	channel = NULL;
	for (size_t i = 0; i < channels_.size(); ++i)
	{
		if (channels_[i] && channels_[i]->getName() == channel_name)
		{
			channel = channels_[i];
			break ;
		}
	}
	if (!channel)
	{
		std::string msg = "403 " + client.getNickName() + " " + channel_name
			+ " :No such channel\r\n";
		send_all(client.getFdSocket(), msg.c_str());
		return ;
	}
	if (!channel->isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + channel_name
			+ " :You're not channel operator\r\n";
		send_all(client.getFdSocket(), msg.c_str());
		return ;
	}
	target = NULL;
	std::string target_nick = target_name;
	for (size_t i = 0; i < clients_.size(); ++i)
	{
		if (clients_[i]->getNickName() == target_nick)
		{
			target = clients_[i];
			break ;
		}
	}
	if (!target)
	{
		std::string error_msg = "401 " + client.getNickName() + " "
			+ target_name + " :No such nick/channel\r\n";
		send_all(client.getFdSocket(), error_msg.c_str());
		return ;
	}
	std::string msg = "You have been invited to " + channel_name + " by "
		+ client.getNickName() + "\r\n";
	channel->addInvite(*target);
	send_all(target->getFdSocket(), msg.c_str());
}
