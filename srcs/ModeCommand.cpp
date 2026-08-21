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
#include <sstream>

std::string intToString(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

ModeCommand::ModeCommand(std::vector<Channel> &channels)
	: channels_(&channels)
{
}

ModeCommand::~ModeCommand()
{
}

std::string ModeCommand::reconstructModes(Channel &channel) const
{
	std::string modes;

	if (channel.isInviteOnly())
		modes += 'i';
	if (channel.isTopicRestricted())
		modes += 't';
	if (channel.hasPassword())
		modes += 'k';
	if (channel.hasUserLimit())
		modes += 'l';
	if (modes.size() > 0)
		modes.insert(0, 1, '+');
	return (modes);
}

std::string ModeCommand::buildModeBroadcast(const std::string &senderPrefix,
                                              const std::string &channelName,
                                              const std::vector<AppliedMode> &applied) const
{
	if (applied.empty())
		return "";

	std::string modestring;
	std::string argsPart;
	char lastSign = 0;

	for (size_t i = 0; i < applied.size(); ++i)
	{
		const AppliedMode &m = applied[i];
		if (m.sign != lastSign)
		{
			modestring += m.sign;
			lastSign = m.sign;
		}
		modestring += m.letter;
		if (m.hasArg)
			argsPart += " " + m.arg;
	}
	return ":" + senderPrefix + " MODE " + channelName + " " + modestring + argsPart + "\r\n";
}

void ModeCommand::broadcastToChannel(Channel &channel, const std::string &msg) const
{
	// Adjust getClients()/getMembers() to whatever your Channel class actually exposes
	std::vector<Client *> members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i)
		send_all(members[i]->getFdSocket(), msg);
}

void ModeCommand::execute(Client &client, const std::string &args)
{
	std::string mode, modeArgs, channelName;
	parseMode(args, channelName, mode, modeArgs);

	std::cout << "[" << client.getNickName() << "] MODE " << mode << std::endl;
	ModeCommand::mode(client, channelName, mode, modeArgs);
}

void ModeCommand::modePrepare(Client &client, const std::string channelName, const std::string &mode_str, const std::string &args)
{
	std::vector<Channel *> channel = client.getChannels();
	if (channel.empty())
	{
		std::string msg = "403 " + client.getNickName() + " * :You are not in a channel\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}

	mode(client, channelName, mode_str, args);
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
	std::cout << "Entered MODE" << std::endl;
	std::cout << "[DEBUG] str-mode is " << mode_str << '\n';
	if (!channel)
	{
		std::string msg = ":localhost 403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}
	if (channel_name != "" && mode_str == "" && args == "")
	{
		std::cout << "Entered channel" << std::endl;
		std::string modes, modeParams;
		if (channel->isInviteOnly())
			modes += 'i';
		if (channel->isTopicRestricted())
			modes += 't';
		if (channel->hasPassword())
		{
			modes += 'k';
			if (channel->findClientByNickname(client.getNickName()) && channel->isOperator(client))
				modeParams += "password is " + channel->getPassword() + ' ';
		}
		if (channel->hasUserLimit())
		{
			modes += 'l';
			modeParams += "user limit is " + intToString(channel->getUserLimit());
		}
		std::string msg = ":localhost 324 " + client.getNickName() + " " + channel_name + " " + reconstructModes(*channel) + " " + modeParams + "\r\n";
		send_all(client.getFdSocket(), msg);
		return ;
	}
	if (!channel->findClientByNickname(client.getNickName()))
	{
		std::string msg = ":localhost 442 " + client.getNickName() + " " + channel_name + " :You're not on that channel\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}
	if (!channel->isOperator(client))
	{
		std::string msg = ":localhost 482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n";
		send_all(client.getFdSocket(), msg);
		return;
	}
	unsigned int sign = UNSIGNED;
	if (mode_str.empty())
		return;
	std::istringstream iss(args);
	std::string token;
	std::vector<std::string> tokens;
	while (iss >> token) // >> already skips consecutive whitespace
    	tokens.push_back(token);
	size_t argsIndex = 0;
	std::vector<AppliedMode> applied;
	for (size_t current = 0; current < mode_str.length(); current++)
	{
		while (mode_str[current] == '+' || mode_str[current] == '-')
		{
			if (mode_str[current] == '+')
			{
				sign = PLUS;
				current++;
			}
			else if (mode_str[current] == '-')
			{
				sign = MINUS;
				current++;
			}
		}
		std::cout << "Entered flag loop" << std::endl;
		char mode = mode_str[current];
		switch (mode)
		{
			case 't':
				channel->setTopicRestricted(sign);
				std::cout << "[" << client.getNickName() << "] Set topic restriction: " << (sign ? "ON" : "OFF") << std::endl;
				{
					AppliedMode a = { sign ? '+' : '-', 't', "", false };
					applied.push_back(a);
				}
				break;
			case 'i':
				channel->setInviteOnly(sign);
				std::cout << "[" << client.getNickName() << "] Set invite only: " << (sign ? "ON" : "OFF") << std::endl;
				{
					AppliedMode a = { sign ? '+' : '-', 'i', "", false };
					applied.push_back(a);
				}
				break;
			case 'o':
			{
				if (argsIndex >= tokens.size())
				{
					std::string msg = ":localhost 461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
					send_all(client.getFdSocket(), msg);
					return;
				}
				Client *target = channel->findClientByNickname(tokens[argsIndex]);
				argsIndex++;
				if (!target)
				{
					std::string msg = ":localhost 401 " + client.getNickName() + " " + args + " :No such nick/channel\r\n";
					send_all(client.getFdSocket(), msg);
					return;
				}
				if (sign)
				{
					channel->addOperator(*target);
					// Notifier le canal du changement de mode
					std::string modeMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost MODE " + channel_name + " +o " + args + "\r\n";
					channel->broadcastToAll(modeMsg);
					std::cout << "[" << client.getNickName() << "] MODE +o " << args << std::endl;
				}
				else
				{
					channel->removeOperator(*target);
					// Notifier le canal du changement de mode
					std::string modeMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost MODE " + channel_name + " -o " + args + "\r\n";
					channel->broadcastToAll(modeMsg);
					std::cout << "[" << client.getNickName() << "] MODE -o " << args << std::endl;
				}
				{
					AppliedMode a = { sign ? '+' : '-', 'o', target->getNickName(), true };
					applied.push_back(a);
				}
				break;
			}
			case 'k':
			{
				if (sign)
				{
					if (argsIndex >= tokens.size())
					{
						std::string msg = ":localhost 461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
						send_all(client.getFdSocket(), msg);
						return;
					}
					channel->setPassword(tokens[argsIndex]);
					{
						AppliedMode a = { '+', 'k', tokens[argsIndex], true };
						applied.push_back(a);
					}
					argsIndex++;
					std::cout << "[" << client.getNickName() << "] MODE +k " << args << std::endl;
				}
				else
				{
					channel->removePassword();
					{
						AppliedMode a = { '-', 'k', "", false };
						applied.push_back(a);
					}
					std::cout << "[" << client.getNickName() << "] MODE -k" << std::endl;
				}
				break;
			}
			case 'l':
			{
				if (sign)
				{
					if (argsIndex >= tokens.size())
					{
						std::cout << "tokens size is " << intToString(tokens.size()) << '\n';
						std::string msg = ":localhost 461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
						send_all(client.getFdSocket(), msg);
						return;
					}
					int limit = std::atoi(tokens[argsIndex].c_str());
					argsIndex++;
					if (limit <= 0)
					{
						std::string msg = ":localhost 461 " + client.getNickName() + " MODE :Invalid limit\r\n";
						send_all(client.getFdSocket(), msg);
						return;
					}
					channel->setUserLimit(limit);
					{
						AppliedMode a = { '+', 'l', intToString(limit), true };
						applied.push_back(a);
					}
					std::cout << "[" << client.getNickName() << "] MODE +l " << limit << std::endl;
				}
				else
				{
					channel->removeUserLimit();
					{
						AppliedMode a = { '-', 'l', "", false };
						applied.push_back(a);
					}
					std::cout << "[" << client.getNickName() << "] MODE -l" << std::endl;
				}
				break;
			}
			default:
			{
				std::string msg = ":localhost 472 " + client.getNickName() + " " + mode + " :Is unknown char to me \r\n";
				send_all(client.getFdSocket(), msg);
				break;
			}
		}
	}
	std::string senderPrefix = ':' + client.getNickName() + "!" + client.getUserName() + "@" + "localhost" ;
	std::string broadcast = buildModeBroadcast(senderPrefix, channel_name, applied);
	if (!broadcast.empty())
		broadcastToChannel(*channel, broadcast);
}
