/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

// Forward declarations for handlers
void handleNewConnection(int sockfd, struct sockaddr_in &cli_addr, socklen_t &clilen,
						 struct pollfd *fds, std::vector<Client> &clients, int maxClients);
void handleClientDisconnection(struct pollfd *fds, std::vector<Client> &clients, size_t clientIdx);
void handleClientInput(std::vector<Client> &clients, size_t clientIdx,
					   char *buffer, int n, Commande &commande);

int main(int argc, char *argv[])
{
	const int maxClients = 5;
	int sockfd;
	char buffer[256];
	struct pollfd *fds;
	int n;

	std::vector<Client> clients;
	std::vector<Channel> channels;

	clients.reserve(maxClients);
	channels.reserve(20);

	channels.push_back(Channel("#general", "Hello World!"));
	channels.push_back(Channel("#42", "Hello World!"));
	channels.push_back(Channel("#test", "Just a test channel"));
	channels.push_back(Channel("#students", "a channel dedicated to exchanging between students"));

	Commande commande(clients, channels);

	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	Serveur serveur(atoi(argv[1]), maxClients);
	serveur.initialize();

	sockfd = serveur.getSockFd();
	fds = serveur.getFds();
	socklen_t &clilen = serveur.getCliLen();
	struct sockaddr_in &cli_addr = serveur.getCliAddr();

	while (1)
	{
		bzero(buffer, 256);

		if (poll(fds, clients.size() + 1, 100) > 0)
		{
			if ((fds[0].revents & POLLIN) && (int)clients.size() + 1 <= maxClients)
			{
				if (clients.size() >= (size_t)maxClients)
					break;

				int newFd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
				if (newFd < 0)
					break;
				;

				int flags = fcntl(newFd, F_GETFL, 0);
				fcntl(newFd, F_SETFL, flags | O_NONBLOCK);

				write(newFd, "Please enter your username:\r\n", 29);

				Client newClient;
				newClient.setFdSocket(newFd);
				clients.push_back(newClient);

				fds[clients.size()].fd = newFd;
				fds[clients.size()].events = POLLIN;
			}

			for (unsigned long i = 1; i < clients.size() + 1; i++)
			{
				if ((fds[i].revents & POLLIN) && fds[i].fd != -2)
				{
					n = read(fds[i].fd, buffer, 255);

					if (n < 0)
					{
						if (errno != EAGAIN && errno != EWOULDBLOCK)
							error("ERROR reading from socket");
						continue;
					}

					if (n == 0)
					{
						std::cout << "destructor Client" << std::endl;
						if (clients[i - 1].getChannel() != NULL)
							clients[i - 1].getChannel()->leave(clients[i - 1]);

						close(fds[i - 1 + 1].fd);
						fds[i - 1 + 1].fd = -2;
						clients.erase(clients.begin() + i - 1);
						i--;
						continue;
					}

					// handleClientInput(clients, i - 1, buffer, n, commande);
					Client &client = clients[i - 1];

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
								void handleCommand(Client & client, const std::string &line, Commande &commande);
								handleCommand(client, line, commande);
							}
						}
					}
				}
			}
		}
	}

	return 0;
}
