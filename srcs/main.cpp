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
     ++it)
     {
         if (*it == target)
         {
               delete *it;              // destroy object
               it = client.erase(it);  // returns next iterator
          }
          else
               ++it;
     }
}

int main(int argc, char *argv[])
{
	const int maxClients = 5;
     int sockfd;
     char buffer[256];
     std::vector<pollfd> fds;
     int n;
     std::vector<Client *> client;
     std::vector<Channel> channels;

	 client.reserve(maxClients);
	 channels.reserve(20);


     channels.push_back(Channel("test", "Just a test channel"));
     channels.push_back(Channel("students", "a channel dedicated to exchanging between students"));

	channels.push_back(Channel("#test", "Just a test channel"));
	channels.push_back(Channel("#students", "a channel dedicated to exchanging between students"));

	Commande commande(client, channels);

	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	Serveur serveur(atoi(argv[1]), maxClients);
	serveur.initialize();

	sockfd = serveur.getSockFd();
	socklen_t &clilen = serveur.getCliLen();
	fds.push_back(newPoll(sockfd));
	struct sockaddr_in &cli_addr = serveur.getCliAddr();

	while (1)
	{
		bzero(buffer, 256);

		if (poll(fds.data(), client.size() + 1, 100) > 0)
		{
			if ((fds[0].revents & POLLIN) && (int)client.size() + 1 <= maxClients)
               {
					if (client.size() >= (size_t)maxClients)
						break;
                    fds.push_back(newPoll(accept(sockfd,
                          (struct sockaddr *) &cli_addr,
                          &clilen)));

					int flags = fcntl(fds.back().fd, F_GETFL, 0);
					fcntl(fds.back().fd, F_SETFL, flags | O_NONBLOCK);
						//check si fd < 0
                    if (fds.back().fd >= 0)
                    {
                         n = write(fds.back().fd,"username :",10);
                         client.push_back(new Client()); //on cree le premier client vide
                         client.back()->setFdSocket(fds.back().fd); //on lui assigne le fd
                       //   if (client.back()->getChannel() == NULL)
                       //        std::cout << "client has no channel at creation" << std::endl;
                    }
               }

			for (unsigned long i = 1; i < client.size() + 1; i++)
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
						close(fds[i].fd); //on ferme le fd
                        fds.erase(fds.begin() + i); //on erase le pollfd du vecteur
                        eraseClient(client, client[i - 1]);
						i--;
						continue;
					}

					client[i - 1]->accessBuffer() += std::string(buffer, n);
                    size_t pos;
					while ((pos = client[i - 1]->getInput().find('\n')) != std::string::npos) // tant qu'il y a un \n dans le read
                    {
                        std::string line = client[i - 1]->getInput().substr(0, pos); //line == la ligne jusqu'au _n
                        client[i - 1]->getInput().erase(0, pos + 1);
                        trim(line);
						if (!line.empty())
						{
							std::cout << "[SERVER] Raw input: '" << line << "'" << std::endl;
                        	if (client[i - 1]->getNameStatus() == false) // si le client a pas de nom on remplit
                        	{
                        	    client[i - 1]->setUserName(line);
								std::string msg = "Please enter your nickname:\r\n";
								write(client[i - 1]->getFdSocket(), msg.c_str(), msg.size());
                        	    client[i - 1]->setReading(true);
                        	}
							else if (!client[i - 1]->getNicknameStatus())
							{
								client[i - 1]->setNickName(line);
								std::string msg = "Please join a channel using: JOIN #channelname\r\n";
								write(client[i - 1]->getFdSocket(), msg.c_str(), msg.size());
							}
                        	else //sinon on ecrit
                        	{
                        	    void handleCommand(Client & client, const std::string &line, Commande &commande);
								handleCommand(*client[i - 1], line, commande);
                        	}
                        	client[i - 1]->accessBuffer().erase(0, pos + 1); // puis on enleve ce qu'on a ecrit/mis en username
						}
                    }
				}
			}
		}
	}

	return 0;
}
