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

std::string trim(const std::string &str)
{
     std::string result = str;
     for (size_t i = 0; i < result.size(); i++)
     {
          if (result[i] == '\n')
               result[i] = '\0';
     }
     return (result);
}
void writeOnTerm(int fd,char *message, pollfd *fds, Client **client)
{
	int fdsend = 0;
     std::string username;
     if (!client)
          return ;
	for (int i = 1; fds[i].fd != -2; i++)
	{
		if(fds[i].fd == fd)
		 fdsend = i;
	}
	
	for(int i = 1; fds[i].fd != -2; i++)
	{
          if (!client[fdsend])
               continue;
          if(fds[i].fd != fd)
          {
               username = trim(client[fdsend]->getUserName());
               write(fds[i].fd, username.c_str(), username.size());
               write(fds[i].fd, " :", 2);
               write(fds[i].fd, message, strlen(message));
               std::cout << message << std::endl;
          }
	}
}

int main(int argc, char *argv[])
{
	 const int maxClients = 5;
     int sockfd;
     char buffer[256];
     struct pollfd *fds;
     int n;
     std::vector<Client> client;
	//Client **client;
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
	while (1)
	{
          // for (int i = 0; i < client.size() + 1; i++)
          // {
          //      if (fds[i].fd == -2)
          //      {
          //           int tmp;
          //           tmp = accept(sockfd,
          //             (struct sockaddr *) &cli_addr,
          //             &clilen);
          //           if (tmp != -1)
          //                fds[i].fd = tmp;
          //           if (fds[i].fd != -2)
          //                client.size() + 1 ++;
          //           break;
          //      }
          // }
          // std::cout << "newsockfd is " << newsockfd << std::endl;
          // if (newsockfd < 0)
          //      error("ERROR on accept");
          bzero(buffer,256);
          //std::cout << client.size() + 1 << std::endl;
          if (poll(fds, client.size() + 1, 100) > 0) //faire une gestion pour -1 et errno plus tard
          {
               if (fds[0].revents && client.size + 1 <= maxClients)
               {
                    fds[client.size() + 1].fd = accept(sockfd,
                         (struct sockaddr *) &cli_addr,
                         &clilen);;
                         if (fds[client.size() + 1].fd >= 0)
                         {
                              n = write(fds[client.size() + 1].fd,"username :",10);
                              n = read(fds[client.size() + 1].fd, buffer,255);
                              if (n > 0)
                                   buffer[n] = '\0';
                              // if (client[client.size + 1])
                              //      delete client[client.size() + 1];
                              client.push_back(new Client());
                              client.last().initialize(fds[client.size()].fd, buffer);
                              std::cout << client.last().getUserName() << std::endl;
                         }

               }
               for (int i = 1; i < client.size() + 1; i++)
               {
                    if (fds[i].revents == POLLIN && fds[i].fd != -2)
                    {
                         n = read(fds[i].fd, buffer,255);
                         if (n < 0) error("ERROR reading from socket");
                     if (n > 0)
                         buffer[n] = '\0';
                     if (client[i])
                          std::cout << fds[i].fd << client[i]->getUserName() << " message :" << buffer << std::endl;
                     writeOnTerm(fds[i].fd, buffer, fds, client);
					 n = write(fds[i].fd,"[message send]\n", 15);
						 bzero(buffer, 256);
                         // fds[i].revents = 0;
                    }
               }
          }
	}
      Client::destroyPool(client, maxClients);
     return 0;
}
