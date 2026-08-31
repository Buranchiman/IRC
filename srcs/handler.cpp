/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include "../includes/Commande.hpp"
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


void	handleClientDisconnection(std::vector<pollfd> &fds,
		std::vector<Client *> &clients, size_t clientIdx)
{
	if (clients[clientIdx]->getChannels().size() > 0)
	{
		for (std::vector<Channel *>::iterator it = clients[clientIdx]->getChannels().begin(); it != clients[clientIdx]->getChannels().end(); ++it)
			(*it)->leave(*clients[clientIdx]);
	}
	close(fds[clientIdx + 1].fd);
	fds.erase(fds.begin() + clientIdx + 1);
	clients.erase(clients.begin() + clientIdx);
}

void	handleClientLine(Client &client, const std::string &line,
			Commande &commande);

void	handleClientInput(std::vector<Client> &clients, size_t clientIdx,
		char *buffer, int n, Commande &commande)
{
	size_t	pos;

	Client &client = clients[clientIdx];
	client.accessBuffer() += std::string(buffer, n);
	while ((pos = client.accessBuffer().find('\n')) != std::string::npos)
	{
		std::string line = client.accessBuffer().substr(0, pos);
		client.accessBuffer().erase(0, pos + 1);
		if (line.size() > 0 && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		trim(line);
		if (!line.empty())
		{
			handleClientLine(client, line, commande);
		}
	}
}

void	handleClientLine(Client &client, const std::string &line,
		Commande &commande)
{
	if (!client.getNameStatus())
	{
		client.setUserName(line);
		std::string msg = "Please enter your nickname:\r\n";
		send_all(client.getFdSocket(), msg.c_str());
	}
	else if (!client.getNicknameStatus())
	{
		client.setNickName(line);
		std::string msg = "Please join a channel using: JOIN #channelname\r\n";
		send_all(client.getFdSocket(), msg.c_str());
	}
	else
	{
		void handleCommand(Client & client, const std::string &line,
			Commande &commande);
		handleCommand(client, line, commande);
	}
}
