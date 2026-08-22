/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 14:33:43 by luciendacun       #+#    #+#             */
/*   Updated: 2026/08/22 16:02:36 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <poll.h>
#include <netinet/in.h>
#include <vector>
#include <signal.h>
#include <string>

class Serveur
{
private:
	int					_sockfd;
	int					_portno;
	socklen_t			_clilen;
	struct sockaddr_in	_serv_addr;
	struct sockaddr_in	_cli_addr;
	int					_maxClients;
	std::string			_password;
public:
	Serveur();
	Serveur(int portno, std::string password, int maxClients);
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
	std::string			getPassword() const;
};

