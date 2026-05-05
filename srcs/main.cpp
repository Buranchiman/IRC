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
				handleNewConnection(sockfd, cli_addr, clilen, fds, clients, maxClients);
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
						handleClientDisconnection(fds, clients, i - 1);
						i--;
						continue;
					}
					
					handleClientInput(clients, i - 1, buffer, n, commande);
				}
			}
		}
	}
	
	return 0;
}
