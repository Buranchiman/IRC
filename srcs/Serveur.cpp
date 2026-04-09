/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/04/09 15:42:35 by buranchiman      ###   ########.fr       */
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

Serveur::Serveur()
:	_sockfd(-1),
	_portno(0),
	_clilen(0),
	_fds(NULL),
	_maxClients(0)
{
	std::memset(&_serv_addr, 0, sizeof(_serv_addr));
	std::memset(&_cli_addr, 0, sizeof(_cli_addr));
}

Serveur::Serveur(int portno, int maxClients)
:	_sockfd(-1),
	_portno(portno),
	_clilen(0),
	_fds(NULL),
	_maxClients(maxClients)
{
	std::memset(&_serv_addr, 0, sizeof(_serv_addr));
	std::memset(&_cli_addr, 0, sizeof(_cli_addr));
}

Serveur::Serveur(const Serveur &other)
:	_sockfd(-1),
	_portno(0),
	_clilen(0),
	_fds(NULL),
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
		if (_fds)
			delete[] _fds;

		_sockfd = (other._sockfd >= 0) ? dup(other._sockfd) : -1;
		_portno = other._portno;
		_clilen = other._clilen;
		_serv_addr = other._serv_addr;
		_cli_addr = other._cli_addr;
		_maxClients = other._maxClients;
		_fds = NULL;
		if (other._fds && _maxClients >= 0)
		{
			_fds = new struct pollfd[_maxClients + 1];
			for (int i = 0; i <= _maxClients; i++)
				_fds[i] = other._fds[i];
			if (_sockfd >= 0)
				_fds[0].fd = _sockfd;
		}
	}
	return (*this);
}

Serveur::~Serveur()
{
	if (_sockfd >= 0)
		close(_sockfd);
	if (_fds)
		delete[] _fds;
}

void	Serveur::initialize()
{
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}
	_fds = new struct pollfd[_maxClients + 1];
	for (int i = 0; i <= _maxClients; i++)
	{
		_fds[i].fd = -2;
		_fds[i].events = POLLIN;
	}
	_fds[0].fd = _sockfd;
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

struct pollfd	*Serveur::getFds() const
{
	return (_fds);
}

socklen_t	&Serveur::getCliLen()
{
	return (_clilen);
}

struct sockaddr_in	&Serveur::getCliAddr()
{
	return (_cli_addr);
}
