/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luciendacunha <luciendacunha@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:30:30 by luda-cun          #+#    #+#             */
/*   Updated: 2026/04/05 16:41:47 by luciendacun      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Client.hpp"

Client::Client(): userName_(""), fdSocket_(0)
{
	std::cout << "Constructor Client" << std::endl;
}

Client::Client(const Client &other)
{
	*this = other;
}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->userName_ = other.userName_;
		this->fdSocket_ = other.fdSocket_;
	}
	return (*this);
}

Client::~Client()
{
	std::cout << "destructor Client" << std::endl;
}

//setter
void Client::setFdSocket(int fd)
{
	this->fdSocket_ = fd;
}

void Client::setUserName(const char *username)
{
	this->userName_ = username;
}

void Client::reset()
{
	this->fdSocket_ = -1;
	this->userName_.clear();
}

void Client::initialize(int fdSocket, const char *userName)
{
	this->fdSocket_ = fdSocket;
	this->userName_ = userName;
}
//getter

int Client::getFdSocket() const
{
	return (this->fdSocket_);
}

std::string Client::getUserName()
{
	return (this->userName_);
}

Client	**Client::createPool(int maxClients)
{
	Client **clients = new Client*[maxClients + 1];
	for (int i = 0; i <= maxClients; i++)
		clients[i] = NULL;
	return (clients);
}

void	Client::destroyPool(Client **clients, int maxClients)
{
	for (int i = 0; i <= maxClients; i++)
	{
		if (clients[i])
			delete clients[i];
	}
	delete[] clients;
}
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h>

// void error(const char *msg)
// {
//     perror(msg);
//     exit(0);
// }

// int main(int argc, char *argv[])
// {
//     int sockfd, portno, n;
//     struct sockaddr_in serv_addr;
//     struct hostent *server;

//     char buffer[256];
//     if (argc < 3) {
//        fprintf(stderr,"usage %s hostname port\n", argv[0]);
//        exit(0);
//     }
//     portno = atoi(argv[2]);
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0)
//         error("ERROR opening socket");
//     server = gethostbyname(argv[1]);
//     if (server == NULL) {
//         fprintf(stderr,"ERROR, no such host\n");
//         exit(0);
//     }
//     bzero((char *) &serv_addr, sizeof(serv_addr));
//     serv_addr.sin_family = AF_INET;
//     bcopy((char *)server->h_addr,
//          (char *)&serv_addr.sin_addr.s_addr,
//          server->h_length);
//     serv_addr.sin_port = htons(portno);
//     if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
//         error("ERROR connecting");
//     printf("Please enter the message: ");
//     bzero(buffer,256);
//     fgets(buffer,255,stdin);
//     n = write(sockfd,buffer,strlen(buffer));
//     if (n < 0)
//          error("ERROR writing to socket");
//     bzero(buffer,256);
//     n = read(sockfd,buffer,255);
//     if (n < 0)
//          error("ERROR reading from socket");
//     printf("%s\n",buffer);
//     close(sockfd);
//     return 0;
// }
