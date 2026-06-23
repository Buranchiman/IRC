/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luciendacunha <luciendacunha@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:30:30 by luda-cun          #+#    #+#             */
/*   Updated: 2026/06/10 14:01:36 by luciendacun      ###   ########.fr       */
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

Client::Client(): userName_(""), nickName_(""), pendingInput(""), pendingJoin_(""), hasUsername(false), hasNickname(false), hasWelcomeSent(false), fdSocket_(0), channels_()
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
		this->pendingJoin_ = other.pendingJoin_;
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

void Client::setPendingJoin(const std::string &line)
{
	this->pendingJoin_ = line;
}

void Client::clearPendingJoin()
{
	this->pendingJoin_.clear();
}

const std::string &Client::getPendingJoin() const
{
	return (this->pendingJoin_);
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
	// Ajouter le canal à la liste si pas déjà présent
	for (size_t i = 0; i < channels_.size(); ++i)
	{
		if (channels_[i] == channel)
			return; // Déjà dans la liste
	}
	channels_.push_back(channel);
}

void Client::removeChannel(Channel *channel)
{
	// Retirer le canal de la liste
	for (std::vector<Channel *>::iterator it = channels_.begin(); it != channels_.end(); ++it)
	{
		if (*it == channel)
		{
			channels_.erase(it);
			return;
		}
	}
}

void	Client::writeOnTerm(std::string message, Channel *channel)
{
	// Si un canal spécifique est fourni, envoyer à ce canal
	if (channel)
	{
		channel->msgEveryone(*this, message);
	}
	else
	{
		// Sinon, envoyer à TOUS les canaux du client
		for (size_t i = 0; i < channels_.size(); ++i)
		{
			if (channels_[i])
				channels_[i]->msgEveryone(*this, message);
		}
	}
}

Channel *Client::getChannel() const
{
	// Retourner le premier canal (pour compatibilité)
	if (channels_.empty())
		return NULL;
	return channels_[0];
}

const std::vector<Channel *> &Client::getChannels() const
{
	return channels_;
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
