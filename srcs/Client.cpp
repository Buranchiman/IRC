/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chillichien <chillichien@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:30:30 by luda-cun          #+#    #+#             */
/*   Updated: 2026/05/18 12:23:12 by chillichien      ###   ########.fr       */
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

Client::Client(): userName_(""), nickName_(""), pendingInput(""), hasUsername(false), hasNickname(false), hasWelcomeSent(false), fdSocket_(0), channel_(NULL)
{
	// std::cout << "Constructor Client" << std::endl;
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
		this->nickName_ = other.nickName_;
		this->fdSocket_ = other.fdSocket_;
		this->hasUsername = other.hasUsername;
		this->hasNickname = other.hasNickname;
		this->hasWelcomeSent = other.hasWelcomeSent;
		this->channel_ = other.channel_;
	}
	return (*this);
}

Client::~Client()
{
	// std::cout << "destructor Client" << std::endl;
}

//setter
void Client::setFdSocket(int fd)
{
	this->fdSocket_ = fd;
}

void Client::setUserName(std::string username)
{
	this->userName_ = username;
	this->hasUsername = true;
}

void Client::setNickName(std::string nickname)
{
	this->nickName_ = nickname;
	this->hasNickname = true;
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

void Client::setWelcomeSent(bool status)
{
	this->hasWelcomeSent = status;
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

std::string Client::getNickName() const
{
	return (this->nickName_);
}

bool Client::getNameStatus() const
{
	return (this->hasUsername);
}

bool Client::getNicknameStatus() const
{
	return (this->hasNickname);
}

bool Client::getWelcomeSentStatus() const
{
	return (this->hasWelcomeSent);
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
		//std::cout << "Channel of " << userName_ << " exists" << std::endl;
		channel_->msgEveryone(*this, message);
	}
}

void send_all(int fd, const std::string &msg)
{
    size_t sent = 0;
    while (sent < msg.size())
    {
        ssize_t n = send(fd, msg.c_str() + sent,
                         msg.size() - sent, MSG_NOSIGNAL);
        if (n <= 0)
            return;
        sent += n;
    }
}
