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
     channels.push_back(Channel("test", "Just a test channel"));
     channels.push_back(Channel("students", "a channel dedicated to exchanging between students"));
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     Serveur serveur(atoi(argv[1]), maxClients);
     serveur.initialize();
     sockfd = serveur.getSockFd();
	socklen_t &clilen = serveur.getCliLen();
	struct sockaddr_in &cli_addr = serveur.getCliAddr();
     fds.push_back(newPoll(sockfd));
	while (1)
	{
          bzero(buffer,256);
          if (poll(fds.data(), client.size() + 1, 100) > 0) //faire une gestion pour -1 et errno plus tard
          {
               if ((fds[0].revents & POLLIN) && client.size() + 1 <= maxClients)
               {
                    fds.push_back(newPoll(accept(sockfd,
                          (struct sockaddr *) &cli_addr,
                          &clilen)));
                         if (fds.back().fd >= 0)
                         {
                              n = write(fds.back().fd,"username :",10);
                              client.push_back(new Client()); //on cree le premier client vide
                              client.back()->setFdSocket(fds.back().fd); //on lui assigne le fd
                              channels[0].join(*client.back()); //channel par defaut pour l'instant
                              if (client.back()->getChannel() == NULL)
                                   std::cout << "client has no channel at creation" << std::endl;
                         }
               }
               for (unsigned long i = 1; i < fds.size() ; i++)
               {
                    if (fds.size() > 1 && (fds[i].revents & POLLIN) && fds[i].fd != -2)
                    {
                         n = read(fds[i].fd, buffer,255);
                         if (n < 0)
                         {
                              error("ERROR reading from socket");
                         }
                         if (n == 0)
                         {
                              close(fds[i].fd); //on ferme le fd
                              fds.erase(fds.begin() + i); //on erase le pollfd du vecteur
                              eraseClient(client, client[i - 1]); //on erase le pointeur client et le contenu
                              continue;
                         }
                         client[i - 1]->accessBuffer() += std::string(buffer, n);
                         std::cout << "pending input is " << client[i - 1]->getInput() << std::endl;
                         size_t pos;
                         while ((pos = client[i - 1]->getInput().find('\n')) != std::string::npos) // tant qu'il y a un \n dans le read
                         {
                             std::string line = client[i - 1]->getInput().substr(0, pos); //line == la ligne jusqu'au _n
                             client[i - 1]->getInput().erase(0, pos + 1);
                             trim(line);
                             if (client[i - 1]->getNameStatus() == false) // si le client a pas de nom on remplit
                             {
                                 client[i - 1]->setUserName(line);
                                 client[i - 1]->setReading(true);
                             }
                             else //sinon on ecrit
                             {
                                   if (!line.empty())
                                        client[i - 1]->writeOnTerm(line);
                             }
                             client[i - 1]->accessBuffer().erase(0, pos + 1); // puis on enleve ce qu'on a ecrit/mis en username
                         }
                    }
               }
          }
	}
     // delete client;
     return 0;
}
