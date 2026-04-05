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
     int n, nfds;
     nfds = 1;
	 Client **client;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
      Serveur serveur(atoi(argv[1]), maxClients);
     serveur.initialize();
     sockfd = serveur.getSockFd();
     fds = serveur.getFds();
	 client = Client::createPool(maxClients);

	socklen_t &clilen = serveur.getCliLen();
	struct sockaddr_in &cli_addr = serveur.getCliAddr();

	while (1)
	{
          // for (int i = 0; i < nfds; i++)
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
          //                nfds ++;
          //           break;
          //      }
          // }
          // std::cout << "newsockfd is " << newsockfd << std::endl;
          // if (newsockfd < 0)
          //      error("ERROR on accept");
          bzero(buffer,256);
          //std::cout << nfds << std::endl;
          if (poll(fds, nfds, 100) > 0) //faire une gestion pour -1 et errno plus tard
          {
               if (fds[0].revents && nfds <= maxClients)
               {
                    fds[nfds].fd = accept(sockfd,
                         (struct sockaddr *) &cli_addr,
                         &clilen);;
                         if (fds[nfds].fd >= 0)
                         {
                              n = write(fds[nfds].fd,"username :",10);
                              n = read(fds[nfds].fd, buffer,255);
                              if (n > 0)
                                   buffer[n] = '\0';
                              if (client[nfds])
                                   delete client[nfds];
                              client[nfds] = new Client();
                              client[nfds]->initialize(fds[nfds].fd, buffer);
                              std::cout << client[nfds]->getUserName() << std::endl;
                              nfds ++;
                         }

               }
               for (int i = 1; i < nfds; i++)
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
					 n = write(fds[i].fd,"[message send]\n",14);
						 bzero(buffer, 256);
                         // fds[i].revents = 0;
                    }
               }
          }
	}
      Client::destroyPool(client, maxClients);
     return 0;
}
