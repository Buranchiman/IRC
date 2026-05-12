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
     )
     {
         if (*it == target)
         {
               delete *it;              // destroy object
               it = client.erase(it);  // returns next iterator, no need to increment
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

	// Send CAP reply
	std::string cap = "CAP * LS :\r\n";
	write(fd, cap.c_str(), cap.size());

	// Send welcome messages
	std::string msg1 = ":localhost 001 " + nick + " :Welcome to IRC server " + nick + "!" + user + "@localhost\r\n";
	write(fd, msg1.c_str(), msg1.size());

	std::string msg2 = ":localhost 002 " + nick + " :Your host is localhost, running IRCv1.0\r\n";
	write(fd, msg2.c_str(), msg2.size());

	std::string msg3 = ":localhost 003 " + nick + " :This server was created just now\r\n";
	write(fd, msg3.c_str(), msg3.size());

	std::string msg4 = ":localhost 004 " + nick + " localhost IRCv1.0 o i\r\n";
	write(fd, msg4.c_str(), msg4.size());

	std::string msg5 = ":localhost 005 " + nick + " CHANTYPES=# EXTBAN=~ :are supported by this server\r\n";
	write(fd, msg5.c_str(), msg5.size());
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
                         client.push_back(new Client());
                         client.back()->setFdSocket(fds.back().fd);
                       //   if (client.back()->getChannel() == NULL)
                       //        std::cout << "client has no channel at creation" << std::endl;
                    }
               }

			for (unsigned long i = 1; i < fds.size() ; i++)
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
					while ((pos = client[i - 1]->getInput().find('\n')) != std::string::npos)
                    {
                        std::string line = client[i - 1]->getInput().substr(0, pos);
                        client[i - 1]->getInput().erase(0, pos + 1);
                        trim(line);
						if (!line.empty())
						{
							std::cout << "[SERVER] Raw input: '" << line << "'" << std::endl;

							bool isAuthCommand = false;

							// Handle IRC protocol commands
							if (line.find("CAP ") == 0)
							{
								// Send CAP response immediately
								std::string cap = "CAP * LS :\r\n";
								write(client[i - 1]->getFdSocket(), cap.c_str(), cap.size());
								isAuthCommand = true;
							}
							else if (line.find("NICK ") == 0)
							{
								std::string nick = line.substr(5);
								client[i - 1]->setNickName(nick);
								isAuthCommand = true;
							}
							else if (line.find("USER ") == 0)
							{
								std::string user = line.substr(5);
								size_t space = user.find(' ');
								if (space != std::string::npos)
									user = user.substr(0, space);
								client[i - 1]->setUserName(user);
								isAuthCommand = true;
							}

							// Send welcome after NICK and USER are both set
							if (client[i - 1]->getNicknameStatus() && client[i - 1]->getNameStatus() && isAuthCommand)
							{
								sendWelcome(client[i - 1]);
							}
							// Handle other IRC commands only after auth complete
							else if (client[i - 1]->getNicknameStatus() && client[i - 1]->getNameStatus() && !isAuthCommand)
							{
								void handleCommand(Client & client, const std::string &line, Commande &commande);
								handleCommand(*client[i - 1], line, commande);
							}

                        	client[i - 1]->accessBuffer().erase(0, pos + 1);
						}
                    }
				}
			}
		}
	}

	return 0;
}
