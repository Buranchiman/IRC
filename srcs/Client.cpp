/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:30:30 by luda-cun          #+#    #+#             */ 
/*   Updated: 2026/08/11 14:29:01 by luda-cun         ###   ########.fr       */
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

Client::Client(): userName_(""), nickName_(""), pendingInput_(""), hasUsername_(false), hasNickname_(false), hasCapStart_(false), hasCapEnd_(false), welcomeSent_(false), registered_(false), fdSocket_(-2), channels_()
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
		this->hasUsername_ = other.hasUsername_;
		this->hasNickname_ = other.hasNickname_;
		this->welcomeSent_ = other.welcomeSent_;
		this->channels_ = other.channels_;
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
	this->hasUsername_ = true;
}

void Client::setNickName(std::string nickname)
{
	this->nickName_ = nickname;
	this->hasNickname_ = true;
}

void Client::setReading(bool opt)
{
	this->hasUsername_ = opt;
}

void Client::reset()
{
	this->fdSocket_ = -1;
	this->userName_.clear();
}

void Client::setwelcomeSent_(bool status)
{
	this->welcomeSent_ = status;
}

void Client::setregistered_(bool status)
{
	registered_ = status;
}

// void Client::initialize(int fdSocket, const char *userName)
// {
// 	this->fdSocket_ = fdSocket;
// 	this->userName_ = userName;
// 	this->hasUsername_ = true;
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
	return (this->hasUsername_);
}

bool Client::getNicknameStatus() const
{
	return (this->hasNickname_);
}

bool Client::getwelcomeSent_Status() const
{
	return (this->welcomeSent_);
}

std::string Client::getInput() const
{
	return (this->pendingInput_);
}

std::vector<Channel *> &Client::getChannels()
{
	return (channels_);
}

bool		Client::gethasCapStart_() const
{
	return (hasCapStart_);
}

bool		Client::gethasCapEnd_() const
{
	return (hasCapEnd_);
}

bool		Client::getregistered_() const
{
	return (registered_);
}

std::string &Client::accessBuffer()
{
	return (pendingInput_);
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
//pour meiux ajouter les chanel
void Client::addChannel(Channel *channel)
{
	for (size_t i = 0; i < channels_.size(); ++i)
	{
		if (channels_[i] == channel)
			return;
	}
	channels_.push_back(channel);
}
//pour mieux suprimer 
void Client::suppChannel(Channel *channel)
{
	for (unsigned int i = 0; i < channels_.size(); )
	{
		if (channels_[i] == channel)
			channels_.erase(channels_.begin() + i);
		else
			++i;
	}
}
void Client::setCapStart(bool status)
{
	hasCapStart_ = status;
}

void Client::setCapEnd(bool status)
{
	hasCapEnd_ = status;
}

// void	Client::writeOnTerm(std::string message)
// {
// 	if (channels_.size() > 0)
// 	{
// 		//std::cout << "Channel of " << userName_ << " exists" << std::endl;
// 		channels_->msgEveryone(*this, message);
// 	}
// }

Client* findClientByNickname(const std::vector<Client *> &clients, const std::string &nickname)
{
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i] && clients[i]->getNickName() == nickname)
            return clients[i];
    }
    return NULL;
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

void Client::checkRegistration(void)
{
	if (hasUsername_ && hasNickname_ && hasCapEnd_ && hasCapStart_ && welcomeSent_)
		registered_ = true;
}