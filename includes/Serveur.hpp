/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 14:33:43 by luciendacun       #+#    #+#             */
/*   Updated: 2026/04/27 14:34:23 by buranchiman      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <poll.h>
#include <netinet/in.h>
#include <vector>
#include <signal.h>

class Serveur
{
private:
	int					_sockfd;
	int					_portno;
	socklen_t			_clilen;
	struct sockaddr_in	_serv_addr;
	struct sockaddr_in	_cli_addr;
	int					_maxClients;

public:
	Serveur();
	Serveur(int portno, int maxClients);
	Serveur(const Serveur &other);
	Serveur &operator=(const Serveur &other);
	~Serveur();

	void				initialize();
	void 				statusServer();
	int					getSockFd() const;
	pollfd				*getFds();
	const pollfd		*getFds() const;
	socklen_t			&getCliLen();
	struct sockaddr_in	&getCliAddr();
};

