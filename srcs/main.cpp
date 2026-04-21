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
#include <poll.h>
#include "../includes/Client.hpp"
#include "../includes/Serveur.hpp"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// void handleSigint(int sig)
// {
//      (void)sig;

// }

int main(int argc, char *argv[])
{
	const int maxClients = 5;
     int sockfd;
     char buffer[256];
     struct pollfd *fds;
     int n;
     // sig_atomic_t signalReceived = 0;
     std::vector<Client> client;
     std::vector<Channel> channels;
     channels.push_back(Channel("test", "Just a test channel"));
     channels.push_back(Channel("students", "a channel dedicated to exchanging between students"));
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     Serveur serveur(atoi(argv[1]), maxClients);
     serveur.initialize();
     sockfd = serveur.getSockFd();
     fds = serveur.getFds();
	socklen_t &clilen = serveur.getCliLen();
	struct sockaddr_in &cli_addr = serveur.getCliAddr();
     // signal(SIGINT, )
	while (1)
	{
          bzero(buffer,256);
          if (poll(fds, client.size() + 1, 100) > 0) //faire une gestion pour -1 et errno plus tard
          {
               if ((fds[0].revents & POLLIN) && client.size() + 1 <= maxClients)
               {
                    fds[client.size() + 1].fd = accept(sockfd,
                         (struct sockaddr *) &cli_addr,
                         &clilen);;
                         if (fds[client.size() + 1].fd >= 0)
                         {
                              n = write(fds[client.size() + 1].fd,"username :",10);
                              // if (client[client.size + 1])
                              //      delete client[client.size() + 1];
                              client.push_back(Client()); //on cree le premier client vide
                              client.back().setFdSocket(fds[client.size()].fd); //on lui assigne le fd
                              channels[0].join(client.back()); //channel par defaut pour l'instant
                              if (client.back().getChannel() == NULL)
                                   std::cout << "client has no channel at creation" << std::endl;
                         }
               }
               for (unsigned long i = 1; i < client.size() + 1; i++)
               {
                    if ((fds[i].revents & POLLIN) && fds[i].fd != -2)
                    {
                         n = read(fds[i].fd, buffer,255);
                         if (n < 0)
                         {
                              error("ERROR reading from socket");
                         }
                         if (n == 0)
                         {
                              //detruire le client et voir comment gerer le fd
                              continue;
                         }
                         client[i - 1].accessBuffer() += std::string(buffer, n);
                         std::cout << "pending input is " << client[i - 1].getInput() << std::endl;
                         size_t pos;
                         while ((pos = client[i - 1].getInput().find('\n')) != std::string::npos) // tant qu'il y a un \n dans le read
                         {
                             std::string line = client[i - 1].getInput().substr(0, pos); //line == la ligne jusqu'au _n
                             client[i - 1].getInput().erase(0, pos + 1);
                             trim(line);
                             if (client[i - 1].getNameStatus() == false) // si le client a pas de nom on remplit
                             {
                                 client[i - 1].setUserName(line);
                                 client[i - 1].setReading(true);
                             }
                             else //sinon on ecrit
                             {
                                   if (!line.empty())
                                        client[i - 1].writeOnTerm(line);
                             }
                             client[i - 1].accessBuffer().erase(0, pos + 1); // puis on enleve ce qu'on a ecrit/mis en username
                         }
                    }
               }
          }
	}
     return 0;
}
