/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:30:30 by luda-cun          #+#    #+#             */
/*   Updated: 2026/04/27 15:52:39 by buranchiman      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Client.hpp"

void trim(std::string &s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
       	while (!s.empty() && (s[s.size() - 1] == '\n' || s[s.size() - 1] == '\r'))
       		s.erase(s.size() - 1);
    }
}

Client::Client(): userName_(""), pendingInput(""), hasUsername(false), fdSocket_(0), channel_(NULL)
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
		this->hasUsername = other.hasUsername;
		this->channel_ = other.channel_;
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

void Client::setUserName(std::string username)
{
	this->userName_ = username;
}

void Client::setReading(bool opt)
{
	this->hasUsername = opt;
}

void Client::reset()
{
	this->fdSocket_ = -1;
	this->userName_.clear();
}

// void Client::initialize(int fdSocket, const char *userName)
// {
// 	this->fdSocket_ = fdSocket;
// 	this->userName_ = userName;
// 	this->hasUsername = true;
// 	trim(this->userName_);
// }
//getter

int Client::getFdSocket() const
{
	return (this->fdSocket_);
}

std::string Client::getUserName() const
{
	return (this->userName_);
}

bool Client::getNameStatus() const
{
	return (this->hasUsername);
}

std::string Client::getInput() const
{
	return (this->pendingInput);
}

Channel		*Client::getChannel() const
{
	return (channel_);
}

std::string &Client::accessBuffer()
{
	return (pendingInput);
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

void Client::setChannel(Channel *channel)
{
	this->channel_ = channel;
}

void	Client::writeOnTerm(std::string message)
{
	if (channel_)
	{
		std::cout << "Channel of " << userName_ << " exists" << std::endl;
		channel_->msgEveryone(*this, message);
	}
}
