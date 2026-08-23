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

#include "../includes/Client.hpp"
#include "../includes/Commande.hpp"
#include "../includes/InviteCommand.hpp"
#include "../includes/JoinCommand.hpp"
#include "../includes/KickCommand.hpp"
#include "../includes/ModeCommand.hpp"
#include "../includes/PrivMsg.hpp"
#include "../includes/TopicCommand.hpp"
#include "../includes/Who.hpp"
#include "../includes/WhoIs.hpp"
#include <iostream>
#include <unistd.h>

std::string parseCommandArg(const std::string &line, const std::string &prefix,
	std::string &remaining)
{
	size_t	start;

	start = prefix.length();
	std::string args = line.substr(start);
	while (!args.empty() && args[0] == ' ')
		args.erase(0, 1);
	remaining = args;
	return (args);
}

void	handleJoinCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels)
{
	std::string args;
	parseCommandArg(line, "JOIN ", args);
	JoinCommand cmd(channels);
	cmd.execute(client, args);
}

void	handleKickCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels)
{
	std::string args;
	parseCommandArg(line, "KICK ", args);
	KickCommand cmd(channels);
	cmd.execute(client, args);
}

void	handleInviteCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels, std::vector<Client *> &clients)
{
	std::string args;
	parseCommandArg(line, "INVITE ", args);
	InviteCommand cmd(channels, clients);
	cmd.execute(client, args);
}

void	handleTopicCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels)
{
	std::string args;
	parseCommandArg(line, "TOPIC ", args);
	TopicCommand cmd(channels);
	cmd.execute(client, args);
}

void	handleModeCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels)
{
	std::string args;
	parseCommandArg(line, "MODE ", args);
	ModeCommand cmd(channels);
	cmd.execute(client, args);
}

void	handlePingCommand(Client &client, const std::string &line)
{
	std::string args;
	parseCommandArg(line, "PING ", args);
	while (!args.empty() && args[args.length() - 1] == ' ')
		args.erase(args.length() - 1);
	std::string response = ":localhost PONG " + args + "\r\n";
	send_all(client.getFdSocket(), response.c_str());
}

void	handleMessageCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels, std::vector<Client *> &clients)
{
	std::string args;
	parseCommandArg(line, "PRIVMSG ", args);
	PrivMsg cmd(clients, channels);
	cmd.execute(client, args);
}

void	handleWhoIsCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels, std::vector<Client *> &clients)
{
	std::string args;
	parseCommandArg(line, "WHOIS ", args);
	WhoIs cmd(clients, channels);
	cmd.execute(client, args);
}

void	handleWhoCommand(Client &client, const std::string &line,
		std::vector<Channel *> &channels, std::vector<Client *> &clients)
{
	std::string args;
	parseCommandArg(line, "WHO ", args);
	Who cmd(clients, channels);
	cmd.execute(client, args);
}

void	handleCommand(Client &client, const std::string &line,
		Commande &commande)
{
	std::vector<Channel *> &channels = commande.getChannels();
	std::vector<Client *> &clients = commande.getClients();
	if (line.size() >= 5 && line.substr(0, 5) == "PING ")
	{
		handlePingCommand(client, line);
		return ;
	}
	else
	{
		if (line.size() >= 5 && line.substr(0, 5) == "JOIN ")
			handleJoinCommand(client, line, channels);
		else if (line.size() >= 5 && line.substr(0, 5) == "KICK ")
			handleKickCommand(client, line, channels);
		else if (line.size() >= 7 && line.substr(0, 7) == "INVITE ")
			handleInviteCommand(client, line, channels, clients);
		else if (line.size() >= 6 && line.substr(0, 6) == "TOPIC ")
			handleTopicCommand(client, line, channels);
		else if (line.size() >= 5 && line.substr(0, 5) == "MODE ")
			handleModeCommand(client, line, channels);
		else if (line.size() >= 8 && line.substr(0, 8) == "PRIVMSG ")
		{
			handleMessageCommand(client, line, channels, clients);
		}
		else if (line.size() >= 6 && line.substr(0, 6) == "WHOIS ")
		{
			handleWhoIsCommand(client, line, channels, clients);
		}
		else if (line.size() >= 4 && line.substr(0, 4) == "WHO ")
		{
			handleWhoCommand(client, line, channels, clients);
		}
	}
}
