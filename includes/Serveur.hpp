/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luciendacunha <luciendacunha@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 14:33:43 by luciendacun       #+#    #+#             */
/*   Updated: 2026/04/05 17:14:10 by luciendacun      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <poll.h>
#include <netinet/in.h>

class Serveur
{
private:
	int					_sockfd;
	int					_portno;
	socklen_t			_clilen;
	struct sockaddr_in	_serv_addr;
	struct sockaddr_in	_cli_addr;
	struct pollfd		*_fds; //attention au leak sur la copie 
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
	struct pollfd		*getFds() const;
	socklen_t			&getCliLen();
	struct sockaddr_in	&getCliAddr();
};

