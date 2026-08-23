/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/08/23 14:49:28 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Serveur.hpp"
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>

Serveur::Serveur()
:	_sockfd(-1),
	_portno(0),
	_clilen(0),
	_maxClients(0)
{
	std::memset(&_serv_addr, 0, sizeof(_serv_addr));
	std::memset(&_cli_addr, 0, sizeof(_cli_addr));
}

Serveur::Serveur(int portno, std::string password, int maxClients)
:	_sockfd(-1),
	_portno(portno),
	_clilen(0),
	_maxClients(maxClients),
	_password(password)
{
	std::memset(&_serv_addr, 0, sizeof(_serv_addr));
	std::memset(&_cli_addr, 0, sizeof(_cli_addr));
}

Serveur::Serveur(const Serveur &other)
:	_sockfd(-1),
	_portno(0),
	_clilen(0),
	_maxClients(0)
{
	*this = other;
}

Serveur &Serveur::operator=(const Serveur &other)
{
	if (this != &other)
	{
		if (_sockfd >= 0)
			close(_sockfd);
		_sockfd = (other._sockfd >= 0) ? dup(other._sockfd) : -1;
		_portno = other._portno;
		_clilen = other._clilen;
		_serv_addr = other._serv_addr;
		_cli_addr = other._cli_addr;
		_maxClients = other._maxClients;
	}
	return (*this);
}

Serveur::~Serveur()
{
	if (_sockfd >= 0)
		close(_sockfd);
}

void	Serveur::initialize()
{
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}
	int reuse = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR) failed");
	}
	
	int flags = fcntl(_sockfd, F_GETFL, 0);
	fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);
	std::memset(&_serv_addr, 0, sizeof(_serv_addr));
	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	_serv_addr.sin_port = htons(_portno);
	if (bind(_sockfd, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) < 0)
	{
		perror("ERROR on binding");
		exit(1);
	}
	listen(_sockfd, 5);
	_clilen = sizeof(_cli_addr);
}

int	Serveur::getSockFd() const
{
	return (_sockfd);
}

socklen_t	&Serveur::getCliLen()
{
	return (_clilen);
}

struct sockaddr_in	&Serveur::getCliAddr()
{
	return (_cli_addr);
}

std::string	Serveur::getPassword() const
{
	return (_password);
}