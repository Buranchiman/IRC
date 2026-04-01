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
void writeOnTerm(int fd,char *message, pollfd *fds, Client *client)
{
	(void)client;
	int fdsend = 0;
     std::string username;
	for (int i = 1; fds[i].fd != -2; i++)
	{
		if(fds[i].fd == fd)
		 fdsend = i;
	}
	
	for(int i = 1; fds[i].fd != -2; i++)
	{
		if(fds[i].fd != fd)
		{
        username = trim(client[fdsend].getUserName());
        write(fds[i].fd, username.c_str(), username.size());
		write(fds[i].fd, " :", 2);
		write(fds[i].fd,message,strlen(message));
		std::cout << message << std::endl;
		}
	}
}

int main(int argc, char *argv[])
{

     int sockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     struct pollfd *fds = new struct pollfd[5]; //reflechir a comment rendre la taille dynamique ??
     int n, nfds;
     nfds = 1;
	 Client client[6];
     for (int i = 0; i < 5; i++)
     {
          fds[i].fd = -2;
          fds[i].events = POLLIN;
          // fds[i].revents = 0;
     }
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     fds[0].fd = sockfd;
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5); //changer pour la macro maximum de listen.h ?
     clilen = sizeof(cli_addr);

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
               if (fds[0].revents && nfds <= 5)
               {
                    fds[nfds].fd = accept(sockfd,
                         (struct sockaddr *) &cli_addr,
                         &clilen);;
					n = write(fds[nfds].fd,"username :",10);
					n = read(fds[nfds].fd, buffer,255);
					client[nfds].setUserName(buffer);
					std::cout << client[nfds].getUserName() << std::endl;
                    nfds ++;

               }
               for (int i = 1; i < nfds; i++)
               {
                    if (fds[i].revents == POLLIN && fds[i].fd != -2)
                    {
                         n = read(fds[i].fd, buffer,255);
                         if (n < 0) error("ERROR reading from socket");
						 std::cout << fds[i].fd << client[i].getUserName() << " message :" << buffer << std::endl;
						 writeOnTerm(fds[i].fd, buffer, fds, client);
                         n = write(fds[i].fd,"message send\n",13);
						 bzero(buffer, 256);
                         // fds[i].revents = 0;
                    }
               }
          }
	}
     close(sockfd);
     return 0;
}
