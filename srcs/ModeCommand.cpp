/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ModeCommand.hpp"
#include "../includes/Commande.hpp"
#include "../includes/Client.hpp"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

static void	sendModeList(Client &client, Channel &channel)
{
	std::string msg = ":localhost 324 " + client.getNickName() + " "
		+ channel.getName() + " " + channel.getModeString() + "\r\n";
	send_all(client.getFdSocket(), msg);
}

ModeCommand::ModeCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

ModeCommand::~ModeCommand()
{
}

void ModeCommand::execute(Client &client, const std::string &args)
{
	std::string channel_name, mode_str, mode_args;
	parseMode(args, channel_name, mode_str, mode_args);

	if (channel_name.empty())
	{
		Channel *current = client.getChannel();
		if (!current)
			return;
		channel_name = current->getName();
	}

	if (mode_str.empty())
	{
		for (size_t i = 0; i < channels_->size(); ++i)
		{
			if ((*channels_)[i].getName() != channel_name)
				continue;
			if (!(*channels_)[i].isMember(client))
				return;
			sendModeList(client, (*channels_)[i]);
			return;
		}
		return;
	}

	std::cout << "[" << client.getNickName() << "] MODE " << channel_name << " " << mode_str << std::endl;
	mode(client, channel_name, mode_str, mode_args);
}

void ModeCommand::mode(Client &client, const std::string &channel_name, const std::string &mode_str, const std::string &args)
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

	if (!channel->isMember(client))
	{
		std::string msg = "442 " + client.getNickName() + " " + channel_name + " :You're not on that channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	if (!channel->isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	if (mode_str.empty() || mode_str[0] != '+')
	{
		std::string msg = "461 " + client.getNickName() + " MODE :Modes must start with +\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	size_t current = 1;
	while (current < mode_str.length())
	{
		char mode = mode_str[current];
		switch (mode)
		{
			case 't':
				channel->setTopicRestricted(!channel->isTopicRestricted());
				std::cout << "[" << client.getNickName() << "] Topic restriction: "
					<< (channel->isTopicRestricted() ? "ON" : "OFF") << std::endl;
				break;
			case 'i':
				channel->setInviteOnly(!channel->isInviteOnly());
				std::cout << "[" << client.getNickName() << "] Invite only: "
					<< (channel->isInviteOnly() ? "ON" : "OFF") << std::endl;
				break;
			case 'o':
			{
				if (args.empty())
				{
					std::string msg = "461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
					write(client.getFdSocket(), msg.c_str(), msg.size());
					return;
				}

				Client *target = channel->findClientByNickname(args);
				if (!target)
				{
					std::string msg = "401 " + client.getNickName() + " " + args + " :No such nick/channel\r\n";
					write(client.getFdSocket(), msg.c_str(), msg.size());
					return;
				}

				if (channel->isOperator(*target))
				{
					channel->removeOperator(*target);
					std::cout << "[" << client.getNickName() << "] MODE +o (off) " << args << std::endl;
				}
				else
				{
					channel->addOperator(*target);
					std::cout << "[" << client.getNickName() << "] MODE +o (on) " << args << std::endl;
				}
				break;
			}
			case 'k':
			{
				if (channel->hasPassword())
				{
					channel->removePassword();
					std::cout << "[" << client.getNickName() << "] MODE +k (off)" << std::endl;
				}
				else
				{
					if (args.empty())
					{
						std::string msg = "461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
						write(client.getFdSocket(), msg.c_str(), msg.size());
						return;
					}
					channel->setPassword(args);
					std::cout << "[" << client.getNickName() << "] MODE +k (on) " << args << std::endl;
				}
				break;
			}
			case 'l':
			{
				if (channel->hasUserLimit())
				{
					channel->removeUserLimit();
					std::cout << "[" << client.getNickName() << "] MODE +l (off)" << std::endl;
				}
				else
				{
					if (args.empty())
					{
						std::string msg = "461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
						write(client.getFdSocket(), msg.c_str(), msg.size());
						return;
					}
					int limit = std::atoi(args.c_str());
					if (limit <= 0)
					{
						std::string msg = "461 " + client.getNickName() + " MODE :Invalid limit\r\n";
						write(client.getFdSocket(), msg.c_str(), msg.size());
						return;
					}
					channel->setUserLimit(limit);
					std::cout << "[" << client.getNickName() << "] MODE +l (on) " << limit << std::endl;
				}
				break;
			}
			default:
				break;
		}
		current++;
	}
}
