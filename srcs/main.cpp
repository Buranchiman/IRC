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

// void writeOnTerm(int fd, std::string message, pollfd *fds, std::vector<Client> &client)
// {
// 	int fdsend = 0;
//      std::string username;
//      if (client.empty())
//           return;
// 	for (int i = 0; fds[i].fd != -2; i++)
// 	{
// 		if(fds[i].fd == fd)
// 		 fdsend = i;           //a verifier pourquoi fdsend = i et pas i - 1 alors que normalement il y a un client de moins que de fds
// 	}

// 	for(int i = 1; fds[i].fd != -2; i++)
// 	{
//           //  if (!client[fdsend].empty())
//           //       continue;
//           if(fds[i].fd != fd)
//           {
//                std::string username = client[fdsend - 1].getUserName();
//                std::cout << "local username is " << username << std::endl;
//                std::string out = username + " :" + std::string(message) + "\n";
//                write(fds[i].fd, out.c_str(), out.size());
//                std::cout << out << std::endl;
//           }
// 	}
// }

int main(int argc, char *argv[])
{
	 const int maxClients = 5;
     int sockfd;
     char buffer[256];
     struct pollfd *fds;
     int n;
     std::vector<Client> client;
     Channel channel("test", "Just a test channel");
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
          bzero(buffer,256);
          // size_t tmp;
          if (poll(fds, client.size() + 1, 100) > 0) //faire une gestion pour -1 et errno plus tard
          {
               // tmp = client.size();
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
                              channel.join(client.back()); //channel par defaut pour l'instant
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
                              error("ERROR reading from socket");

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
