/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandhandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "../includes/Client.hpp"
#include "../includes/Commande.hpp"
#include "../includes/JoinCommand.hpp"
#include "../includes/KickCommand.hpp"
#include "../includes/InviteCommand.hpp"
#include "../includes/TopicCommand.hpp"
#include "../includes/ModeCommand.hpp"

std::string parseCommandArg(const std::string &line, const std::string &prefix, std::string &remaining)
{
	size_t start = prefix.length();
	std::string args = line.substr(start);

	while (!args.empty() && args[0] == ' ')
		args.erase(0, 1);

	remaining = args;
	return args;
}

void handleJoinCommand(Client &client, const std::string &line, std::vector<Channel> &channels)
{
	std::string args;
	parseCommandArg(line, "JOIN ", args);

	JoinCommand cmd(channels);
	cmd.execute(client, args);
}

void handleKickCommand(Client &client, const std::string &line, std::vector<Channel> &channels)
{
	std::string args;
	parseCommandArg(line, "KICK ", args);

	KickCommand cmd(channels);
	cmd.execute(client, args);
}

void handleInviteCommand(Client &client, const std::string &line, std::vector<Channel> &channels, std::vector<Client *> &clients)
{
	std::string args;
	parseCommandArg(line, "INVITE ", args);

	InviteCommand cmd(channels, clients);
	cmd.execute(client, args);
}

void handleTopicCommand(Client &client, const std::string &line, std::vector<Channel> &channels)
{
	std::string args;
	parseCommandArg(line, "TOPIC ", args);

	TopicCommand cmd(channels);
	cmd.execute(client, args);
}

void handleModeCommand(Client &client, const std::string &line, std::vector<Channel> &channels)
{
	std::string args;
	parseCommandArg(line, "MODE ", args);

	ModeCommand cmd(channels);
	cmd.execute(client, args);
}

void handlePingCommand(Client &client, const std::string &line)
{
	std::string args;
	parseCommandArg(line, "PING ", args);

	// Remove trailing whitespace
	while (!args.empty() && args[args.length() - 1] == ' ')
		args.erase(args.length() - 1);

	std::string response = ":localhost PONG " + args + "\r\n";
	write(client.getFdSocket(), response.c_str(), response.size());
}

void handleCapCommand(Client &client, const std::string &line)
{
	std::string args;
	parseCommandArg(line, "CAP ", args);

	if (args.find("LS") != std::string::npos)
	{
		std::string response = "CAP * LS :\r\n";
		write(client.getFdSocket(), response.c_str(), response.size());
	}
	else if (args.find("END") != std::string::npos)
	{
		// Removed slow cout
	}
}

void handleMessageCommand(Client &client, const std::string &line)
{
	// Format: PRIVMSG #channel :message
	// ou: PRIVMSG nickname :message
	
	std::string temp = line.substr(8); // Retirer "PRIVMSG "
	size_t spacePos = temp.find(' ');
	
	if (spacePos == std::string::npos)
		return; // Format invalide
	
	std::string target = temp.substr(0, spacePos);
	
	// Chercher le canal cible
	Channel *targetChannel = NULL;
	if (target[0] == '#')
	{
		// C'est un canal
		for (std::vector<Channel *>::const_iterator it = client.getChannels().begin();
			 it != client.getChannels().end(); ++it)
		{
			if (*it && (*it)->getName() == target)
			{
				targetChannel = *it;
				break;
			}
		}
	}
	
	// Envoyer le message au canal cible
	if (targetChannel)
	{
		client.writeOnTerm(line, targetChannel);
	}
}

void handleCommand(Client &client, const std::string &line, Commande &commande)
{
	std::vector<Channel> &channels = *commande.getChannels();
	std::vector<Client *> &clients = commande.getClients();

	// Handle commands that can be sent anytime (before JOIN)
	if (line.size() >= 5 && line.substr(0, 5) == "PING ")
	{
		handlePingCommand(client, line);
		return;
	}
	if (line.size() >= 4 && line.substr(0, 4) == "CAP ")
	{
		handleCapCommand(client, line);
		return;
	}

	if (line.size() >= 5 && line.substr(0, 5) == "JOIN ")
	{
		handleJoinCommand(client, line, channels);
		return;
	}
	if (line.size() >= 5 && line.substr(0, 5) == "MODE ")
	{
		handleModeCommand(client, line, channels);
		return;
	}

	if (client.getChannel() == NULL)
	{
		if (line.size() > 0)
		{
			std::string msg = "You must join a channel first. Use: JOIN #channelname\r\n";
			write(client.getFdSocket(), msg.c_str(), msg.size());
		}
		return;
	}
	if (line.size() >= 5 && line.substr(0, 5) == "KICK ")
			handleKickCommand(client, line, channels);
		else if (line.size() >= 7 && line.substr(0, 7) == "INVITE ")
			handleInviteCommand(client, line, channels, clients);
	else if (line.size() >= 6 && line.substr(0, 6) == "TOPIC ")
		handleTopicCommand(client, line, channels);
	else if (line.size() >= 8 && line.substr(0, 8) == "PRIVMSG ")
		handleMessageCommand(client, line);
}
