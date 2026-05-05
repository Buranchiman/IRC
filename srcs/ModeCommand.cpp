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
#include <iostream>
#include <unistd.h>
#include <cstdlib>

ModeCommand::ModeCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

ModeCommand::~ModeCommand()
{
}

void ModeCommand::execute(Client &client, const std::string &args)
{
	std::string mode, modeArgs;
	parseMode(args, mode, modeArgs);
	
	std::cout << "[" << client.getNickName() << "] MODE " << mode << std::endl;
	ModeCommand::mode(client, mode, modeArgs);
}

void ModeCommand::mode(Client &client, const std::string &mode_str, const std::string &args)
{
	Channel *channel = client.getChannel();
	if (!channel)
	{
		std::string msg = "403 " + client.getNickName() + " * :You are not in a channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}
	mode(client, channel->getName(), mode_str, args);
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

	if (!channel->isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	bool add = true;
	size_t current = 0;
	if (mode_str.empty())
		return;
	
	if (mode_str[0] == '+')
	{
		add = true;
		current = 1;
	}
	else if (mode_str[0] == '-')
	{
		add = false;
		current = 1;
	}

	while (current < mode_str.length())
	{
		char mode = mode_str[current];
		switch (mode)
		{
			case 't':
				channel->setTopicRestricted(add);
				std::cout << "[" << client.getNickName() << "] Set topic restriction: " << (add ? "ON" : "OFF") << std::endl;
				break;
			case 'i':
				channel->setInviteOnly(add);
				std::cout << "[" << client.getNickName() << "] Set invite only: " << (add ? "ON" : "OFF") << std::endl;
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
				
				if (add)
				{
					channel->addOperator(*target);
					std::cout << "[" << client.getNickName() << "] MODE +o " << args << std::endl;
				}
				else
				{
					channel->removeOperator(*target);
					std::cout << "[" << client.getNickName() << "] MODE -o " << args << std::endl;
				}
				break;
			}
			case 'k':
			{
				if (add)
				{
					if (args.empty())
					{
						std::string msg = "461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
						write(client.getFdSocket(), msg.c_str(), msg.size());
						return;
					}
					channel->setPassword(args);
					std::cout << "[" << client.getNickName() << "] MODE +k " << args << std::endl;
				}
				else
				{
					channel->removePassword();
					std::cout << "[" << client.getNickName() << "] MODE -k" << std::endl;
				}
				break;
			}
			case 'l':
			{
				if (add)
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
					std::cout << "[" << client.getNickName() << "] MODE +l " << limit << std::endl;
				}
				else
				{
					channel->removeUserLimit();
					std::cout << "[" << client.getNickName() << "] MODE -l" << std::endl;
				}
				break;
			}
			default:
				break;
		}
		current++;
	}
}
