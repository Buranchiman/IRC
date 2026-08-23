/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 14:45:04 by luda-cun          #+#    #+#             */
/*   Updated: 2026/08/23 18:12:19 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include "../includes/Client.hpp"
#include "../includes/Serveur.hpp"
#include "../includes/Commande.hpp"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void handleNewConnection(int sockfd, struct sockaddr_in &cli_addr, socklen_t &clilen,
						 struct pollfd *fds, std::vector<Client> &clients, int maxClients);
void handleClientDisconnection(std::vector<pollfd> &fds, std::vector<Client *> &clients, size_t clientIdx);
void handleClientInput(std::vector<Client> &clients, size_t clientIdx,
					   char *buffer, int n, Commande &commande);
void acceptConnection(int sockfd, struct sockaddr_in &cli_addr, socklen_t &clilen, 
					   std::vector<pollfd> &fds, std::vector<Client *> &client);
void processClients(std::vector<pollfd> &fds, std::vector<Client *> &client, char *buffer, int BUFFER_SIZE, 
					Commande &commande, const Serveur &serveur);

pollfd    newPoll(int fd)
{
     pollfd tmp;

     tmp.fd = fd;
     tmp.events = POLLIN;
     return (tmp);
}

void eraseClient(std::vector<Client *> &client, Client *target)
{
     for (std::vector<Client*>::iterator it = client.begin();
     it != client.end();
     )
     {
         if (*it == target)
         {
               delete *it;
               it = client.erase(it);  
          }
          else
               ++it;
     }
}

void sendWelcome(Client *client)
{
	std::string nick = client->getNickName();
	std::string user = client->getUserName();
	int fd = client->getFdSocket();
	send_all(fd, ":localhost 001 " + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@localhost\r\n");
	send_all(fd, ":localhost 002 " + nick + " :Your host is localhost, running IRCv1.0\r\n");
	send_all(fd, ":localhost 003 " + nick + " :This server was created just now\r\n");
	send_all(fd, ":localhost 004 " + nick + " localhost IRCv1.0 io io\r\n");
	send_all(fd, ":localhost 005 " + nick + " CHANTYPES=# NICKLEN=30 USERLEN=10 CASEMAPPING=rfc1459 PREFIX=(o)@ :are supported\r\n");
}

void acceptConnection(int sockfd, struct sockaddr_in &cli_addr, socklen_t &clilen, 
					   std::vector<pollfd> &fds, std::vector<Client *> &client)
{
	int newfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if (newfd < 0)
		return;
	int flags = fcntl(newfd, F_GETFL, 0);
	fcntl(newfd, F_SETFL, flags | O_NONBLOCK);
	int opt = 1;
	setsockopt(newfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	fds.push_back(newPoll(newfd));
	client.push_back(new Client());
	client.back()->setFdSocket(newfd);
}

void processClients(std::vector<pollfd> &fds, std::vector<Client *> &client, char *buffer, int BUFFER_SIZE, 
					Commande &commande, const Serveur &serveur)
{
	for (unsigned long i = 1; i < fds.size(); i++)
	{
		if ((fds[i].revents & POLLIN) && fds[i].fd != -2)
		{
			int n = read(fds[i].fd, buffer, BUFFER_SIZE - 1);
			if (n < 0 || n == 0)
			{
				handleClientDisconnection(fds, client, i - 1);
				if (i < fds.size())
					i--;
				continue;
			}
			std::string &clientBuffer = client[i - 1]->accessBuffer();
			clientBuffer += std::string(buffer, n);
			size_t pos;
			while ((pos = clientBuffer.find('\n')) != std::string::npos)
			{
				std::string line = clientBuffer.substr(0, pos);
				clientBuffer.erase(0, pos + 1);
				trim(line);
				if (!line.empty())
				{
					bool isAuth = false;
					if (!client[i - 1]->gethasCapStart_() && line.find("CAP LS") == 0)
					{
						send_all(client[i - 1]->getFdSocket(), ":localhost CAP * LS :\r\n");
						client[i - 1]->setCapStart(true);
						isAuth = true;
					}
					else if (line.find("NICK ") == 0)
					{
						std::string nick = line.substr(5);
						if (findClientByNickname(client, nick))
						{
							send_all(client[i - 1]->getFdSocket(), ":localhost 433 " + nick + " :Nickname is already in use\r\n");
							handleClientDisconnection(fds, client, i - 1);
							break;
						}
						client[i - 1]->setNickName(nick);
						isAuth = true;
					}
					else if (line.find("USER ") == 0)
					{
						std::string user = line.substr(5);
						size_t space = user.find(' ');
						if (space != std::string::npos)
							user = user.substr(0, space);
						client[i - 1]->setUserName(user);
						isAuth = true;
					}
					if (!client[i - 1]->gethasCapEnd_() && line == "CAP END")
					{
						client[i - 1]->setCapEnd(true);
						isAuth = true;
					}
					if (!client[i - 1]->getPasswordStatus_() && line.find("PASS ") == 0)
					{
						std::string pass = line.substr(5);
						if (pass == serveur.getPassword())
							client[i - 1]->setPassword(true);
						else
						{
							send_all(client[i - 1]->getFdSocket(), ":localhost 464 * :Password incorrect\r\n");
							handleClientDisconnection(fds, client, i - 1);
							break;
						}
						isAuth = true;
					}
					if (!client[i - 1]->getwelcomeSent_Status() && client[i - 1]->getNicknameStatus() && client[i - 1]->getNameStatus() && client[i - 1]->getPasswordStatus_())
					{
						sendWelcome(client[i - 1]);
						client[i - 1]->setwelcomeSent_(true);
					}
					client[i - 1]->checkRegistration();
					if (!isAuth)
					{
						if (client[i - 1]->getregistered_())
						{
							void handleCommand(Client & client, const std::string &line, Commande &commande);
							handleCommand(*client[i - 1], line, commande);
						}
						else
						{
							send_all(client[i - 1]->getFdSocket(), ":localhost 451 * :You have not registered\r\n");
						}
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	const int maxClients = 5;
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	std::vector<Client *> client;
	std::vector<Channel *> channels;
	Commande commande(client, channels);

	if (argc < 3)
	{
		fprintf(stderr, "ERROR, incorrect number of arguments\n");
		exit(1);
	}

	Serveur serveur(atoi(argv[1]), argv[2], maxClients);
	serveur.initialize();

	std::vector<pollfd> fds;
	fds.push_back(newPoll(serveur.getSockFd()));
	socklen_t &clilen = serveur.getCliLen();
	struct sockaddr_in &cli_addr = serveur.getCliAddr();
	int sockfd = serveur.getSockFd();

	while (1)
	{
		bzero(buffer, BUFFER_SIZE);
		if (poll(fds.data(), client.size() + 1, 0) > 0)
		{
			if ((fds[0].revents & POLLIN) && (int)client.size() + 1 <= maxClients)
			{
				if (client.size() >= (size_t)maxClients)
					break;
				acceptConnection(sockfd, cli_addr, clilen, fds, client);
			}
			processClients(fds, client, buffer, BUFFER_SIZE, commande, serveur);
		}
	}
	return 0;
}
