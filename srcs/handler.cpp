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

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include "../includes/Client.hpp"
#include "../includes/Commande.hpp"

void handleNewConnection(int sockfd, struct sockaddr_in &cli_addr, socklen_t &clilen,
						 struct pollfd *fds, std::vector<Client> &clients, int maxClients)
{
	if (clients.size() >= (size_t)maxClients)
		return;
	
	int newFd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if (newFd < 0)
		return;
	
	int flags = fcntl(newFd, F_GETFL, 0);
	fcntl(newFd, F_SETFL, flags | O_NONBLOCK);
	
	write(newFd, "Please enter your username:\r\n", 29);
	
	Client newClient;
	newClient.setFdSocket(newFd);
	clients.push_back(newClient);
	
	fds[clients.size()].fd = newFd;
	fds[clients.size()].events = POLLIN;
}

void handleClientDisconnection(struct pollfd *fds, std::vector<Client> &clients, size_t clientIdx)
{
	if (clients[clientIdx].getChannels().size() > 0)
	{
		for (std::vector<Channel *>::iterator it = clients[clientIdx].getChannels().begin(); it != clients[clientIdx].getChannels().end(); ++it)
			(*it)->leave(clients[clientIdx]);
	}
	close(fds[clientIdx + 1].fd);
	fds[clientIdx + 1].fd = -2;
	clients.erase(clients.begin() + clientIdx);
}

void handleClientLine(Client &client, const std::string &line, Commande &commande);

void handleClientInput(std::vector<Client> &clients, size_t clientIdx,
					   char *buffer, int n, Commande &commande)
{
	Client &client = clients[clientIdx];
	
	client.accessBuffer() += std::string(buffer, n);
	
	size_t pos;
	while ((pos = client.accessBuffer().find('\n')) != std::string::npos)
	{
		std::string line = client.accessBuffer().substr(0, pos);
		client.accessBuffer().erase(0, pos + 1);
		
		if (line.size() > 0 && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		
		trim(line);
		
		if (!line.empty())
		{
			std::cout << "[SERVER] Raw input: '" << line << "'" << std::endl;
			handleClientLine(client, line, commande);
		}
	}
}

void handleClientLine(Client &client, const std::string &line, Commande &commande)
{
	if (!client.getNameStatus())
	{
		client.setUserName(line);
		std::string msg = "Please enter your nickname:\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
	}
	else if (!client.getNicknameStatus())
	{
		client.setNickName(line);
		std::string msg = "Please join a channel using: JOIN #channelname\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
	}
	else
	{
		void handleCommand(Client &client, const std::string &line, Commande &commande);
		handleCommand(client, line, commande);
	}
}
