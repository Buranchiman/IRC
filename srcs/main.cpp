/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
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

// Forward declarations for handlers
void handleNewConnection(int sockfd, struct sockaddr_in &cli_addr, socklen_t &clilen,
						 struct pollfd *fds, std::vector<Client> &clients, int maxClients);
void handleClientDisconnection(std::vector<pollfd> &fds, std::vector<Client *> &clients, size_t clientIdx);
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
	// Send the standard welcome (RPL 001-005)
	send_all(fd, ":localhost 001 " + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@localhost\r\n");
	send_all(fd, ":localhost 002 " + nick + " :Your host is localhost, running IRCv1.0\r\n");
	send_all(fd, ":localhost 003 " + nick + " :This server was created just now\r\n");
	send_all(fd, ":localhost 004 " + nick + " localhost IRCv1.0 io io\r\n");
	send_all(fd, ":localhost 005 " + nick + " CHANTYPES=# NICKLEN=30 USERLEN=10 CASEMAPPING=rfc1459 PREFIX=(o)@ :are supported\r\n");
}

int main(int argc, char *argv[])
{
	const int maxClients = 5;
	const int BUFFER_SIZE = 4096;
     int sockfd;
     char buffer[BUFFER_SIZE];
     std::vector<pollfd> fds;
     int n;
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

	sockfd = serveur.getSockFd();
	socklen_t &clilen = serveur.getCliLen();
	fds.push_back(newPoll(sockfd));
	struct sockaddr_in &cli_addr = serveur.getCliAddr();

	while (1)
	{
		bzero(buffer, BUFFER_SIZE);
		if (poll(fds.data(), client.size() + 1, 0) > 0)
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

					// Disable Nagle's algorithm for low latency
					int opt = 1;
					setsockopt(fds.back().fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

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
					printf("recv %i bytes\n", n);
					if (n < 0)
					{

						// if (errno != EAGAIN && errno != EWOULDBLOCK)
						// 	error("ERROR reading from socket");
						std::cout << "i is at " << i << '\n';
						std::cerr << strerror(errno) << '\n';
						std::cout << "destructor Client" << std::endl;
						std::cout << "before disconnect size is" << fds.size() << '\n';
						handleClientDisconnection(fds, client, i - 1);
						std::cout << "after disconnect size is" << fds.size() << '\n';
						if (i < fds.size())
							i--;
						continue;
					}
					if (n == 0)
					{
						std::cout << "i is at " << i << '\n';
						std::cout << "destructor Client" << std::endl;
						std::cout << "before disconnect size is" << fds.size() << '\n';
						handleClientDisconnection(fds, client, i - 1);
						std::cout << "after disconnect size is" << fds.size() << '\n';
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
						std::cout << "line is " << line  << " and index is i=" << (i) << std::endl;
						if (!line.empty())
						{
						// Removed slow cout for performance
						// std::cout << "[SERVER] Raw input: '" << line << "'" << std::endl;
							// Handle IRC protocol commands
							bool isAuth = false;
							if (!client[i - 1]->gethasCapStart_() && line.find("CAP LS") == 0)
							{
								// Send CAP response immediately
								std::string cap = ":localhost CAP * LS :\r\n";
								send_all(client[i - 1]->getFdSocket(), cap);
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
									std::cout << "[DEBUG] wrong pasword and password used is " << pass <<std::endl;
									send_all(client[i - 1]->getFdSocket(), ":localhost 464 * :Password incorrect\r\n");
									handleClientDisconnection(fds, client, i - 1);
									break;
								}
								isAuth = true;
							}
							// Send welcome after NICK and USER are both set (only once)
							if (!client[i - 1]->getwelcomeSent_Status() && client[i - 1]->getNicknameStatus() && client[i - 1]->getNameStatus() && client[i - 1]->getPasswordStatus_())
							{
								sendWelcome(client[i - 1]);
								client[i - 1]->setwelcomeSent_(true);
							}
							// Handle other IRC commands only after auth complete
							client[i - 1]->checkRegistration();
							if (!isAuth)
							{
								if (client[i - 1]->getregistered_())
								{
									void handleCommand(Client & client, const std::string &line, Commande &commande);
									std::cout << "[" << client[i - 1]->getNickName() << "]" << std::endl;
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
	}
	return 0;
}
