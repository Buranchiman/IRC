#include "Channel.hpp"

Channel::Channel() : name_(""), topic_(""), password_(""), _topicRestricted(false), _inviteOnly(false), _userLimit(-1), clients_(), operators_(), invited_()
{}

Channel::Channel(std::string name, std::string topic) : name_(name), topic_(topic), password_(""), _topicRestricted(false), _inviteOnly(false), _userLimit(-1), clients_(), operators_(), invited_()
{}
Channel::Channel(const Channel& other)
	: name_(other.name_), topic_(other.topic_), password_(other.password_), _topicRestricted(other._topicRestricted), _inviteOnly(other._inviteOnly), _userLimit(other._userLimit),
	  clients_(other.clients_), operators_(other.operators_), invited_(other.invited_)
{
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		name_ = other.name_;
		topic_ = other.topic_;
		password_ = other.password_;
		_topicRestricted = other._topicRestricted;
		_inviteOnly = other._inviteOnly;
		_userLimit = other._userLimit;
		clients_ = other.clients_;
		operators_ = other.operators_;
		invited_ = other.invited_;
	}
	return *this;
}

Channel::~Channel() {
}

void	Channel::join(Client &client)
{
	for (unsigned long i = 0; i < clients_.size(); ++i)
	{
		if (clients_[i] == &client)
			return ;
	}
	clients_.push_back(&client);
	// Le premier client qui rejoint le canal devient opérateur
	if (operators_.empty())
		operators_.push_back(&client);
	else if (client.getUserName() == "operator")
		operators_.push_back(&client);
	client.addChannel(this);
}
//ajiout commande
void	Channel::leave(Client &client)
{
	for (std::vector<Client *>::iterator it = clients_.begin(); it != clients_.end(); ++it)
	{
		if (*it == &client)
		{
			clients_.erase(it);
			break ;
		}
	}
	for (std::vector<Client *>::iterator it = operators_.begin(); it != operators_.end(); ++it)
	{
		if (*it == &client)
		{
			operators_.erase(it);
			break ;
		}
	}
}

void	Channel::msgEveryone(Client &sender, std::string msg)
{
	std::string out = ":" + sender.getNickName() + "!" + sender.getUserName() + "@localhost " + msg + "\r\n";
	for (unsigned long i = 0; i < clients_.size() ; i++)
	{
		if (clients_[i] != &sender)
		{
			std::cout << "name of client is" << clients_[i]->getNickName() << std::endl;
			// envoyer aux autres membres
			send(clients_[i]->getFdSocket(), out.c_str(), out.size(), MSG_NOSIGNAL);
			//write(clients_[i]->getFdSocket(), out.c_str(), out.size());
		}
	}
}

void	Channel::broadcastToAll(std::string msg)
{
	for (unsigned long i = 0; i < clients_.size() ; i++)
	{
		// envoyer le message à tous les membres (le préfixe est déjà inclus par l'appelant)
		write(clients_[i]->getFdSocket(), msg.c_str(), msg.size());
	}
}

std::string const	&Channel::getName() const
{
	return (name_);
}

std::string const	&Channel::getTopic() const
{
	return (topic_);
}

void Channel::setTopic(std::string const &topic, Client &client)
{
	if (_topicRestricted && !isOperator(client))
	{
		std::string msg = "482 " + client.getNickName() + " " + name_ + " :You're not channel operator\r\n";
		write(client.getFdSocket(), msg.c_str(), msg.size());
		return;
	}
	topic_ = topic;
	// Envoi du changement de TOPIC au format IRC standard à tous les membres
	std::string msg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost TOPIC " + name_ + " :" + topic_ + "\r\n";
	broadcastToAll(msg);
	// Also send the topic as a numeric reply to the client who set it
	sendTopic(client);
}

void Channel::sendTopic(Client &client)
{
	std::string topic_msg;
	if (topic_.empty())
	{
		topic_msg = "331 " + client.getNickName() + " " + name_ + " :No topic is set\r\n";
	}
	else
	{
		topic_msg = "332 " + client.getNickName() + " " + name_ + " :" + topic_ + "\r\n";
	}
	write(client.getFdSocket(), topic_msg.c_str(), topic_msg.size());
}

void Channel::setTopicRestricted(bool mode)
{
	_topicRestricted = mode;
	// flag de restriction du topic modifié (pas de log verbeux)
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

bool Channel::isOperator(Client &client)
{
	for (size_t i = 0; i < operators_.size(); i++)
	{
		if (operators_[i] == &client)
			return true;
	}
	return false;
}

void Channel::addOperator(Client &client)
{
	if (!isOperator(client))
	{
		operators_.push_back(&client);
		// notifier le client qu'il est devenu opérateur
		std::string notice = ":localhost NOTICE " + client.getNickName() + " :vous etes op mtn\r\n";
		write(client.getFdSocket(), notice.c_str(), notice.size());
	}
}

void Channel::removeOperator(Client &client)
{
	for (std::vector<Client *>::iterator it = operators_.begin(); it != operators_.end(); ++it)
	{
		if (*it == &client)
		{
			operators_.erase(it);
			return;
		}
	}
}

Client* Channel::findClientByNickname(std::string const &nickname)
{
	for (size_t i = 0; i < clients_.size(); ++i)
	{
		if (clients_[i]->getNickName() == nickname)
		{
			return clients_[i];
		}
	}
	return NULL;
}

void Channel::setInviteOnly(bool mode)
{
	_inviteOnly = mode;
	// mode "invite only" activé/désactivé
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

bool Channel::isInvited(Client &client)
{
	for (size_t i = 0; i < invited_.size(); ++i)
	{
		if (invited_[i] == &client)
			return true;
	}
	return false;
}

void Channel::addInvite(Client &client)
{
	if (!isInvited(client))
	{
		invited_.push_back(&client);
	}
}

void Channel::removeInvite(Client &client)
{
	for (std::vector<Client *>::iterator it = invited_.begin(); it != invited_.end(); ++it)
	{
		if (*it == &client)
		{
			invited_.erase(it);
			return;
		}
	}
}

void Channel::setPassword(std::string const &password)
{
	password_ = password;
	// mot de passe défini pour le channel
}

void Channel::removePassword()
{
	password_ = "";
	// mot de passe supprimé
}

bool Channel::hasPassword() const
{
	return !password_.empty();
}

bool Channel::checkPassword(std::string const &password) const
{
	return password_ == password;
}

void Channel::setUserLimit(int limit)
{
	_userLimit = limit;
	// limite d'utilisateurs définie
}

void Channel::removeUserLimit()
{
	_userLimit = -1;
	// limite d'utilisateurs supprimée
}

bool Channel::hasUserLimit() const
{
	return _userLimit > 0;
}

int Channel::getUserLimit() const
{
	return _userLimit;
}

bool Channel::isUserLimitReached() const
{
	if (!hasUserLimit())
		return false;
	return (int)clients_.size() >= _userLimit;
}

const std::vector<Client*> &Channel::getMembers() const
{
	return clients_;
}

const std::string Channel::getPassword() const
{
	return (password_);
}