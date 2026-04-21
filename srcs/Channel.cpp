#include "Channel.hpp"

Channel::Channel() : name_(""), topic_(""), password_(""),/* topicRestriction_(false), inviteOnly_(false),*/ clients_(), operators_()
{}

Channel::Channel(std::string name, std::string topic) : name_(name), topic_(topic), password_(""),/* topicRestriction_(false), inviteOnly_(false),*/ clients_(), operators_()
{}
Channel::Channel(const Channel& other) {
	(void)other;
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
	}
	return *this;
}

Channel::~Channel() {
}

void	Channel::join(Client &client)
{
	clients_.push_back(&client);
	if (client.getUserName() == "operator")
		operators_.push_back(&client);
	client.setChannel(this);
}

void	Channel::msgEveryone(Client *sender, std::string msg)
{
	std::string out = sender->getUserName() + " :" + std::string(msg) + "\n";
	for (unsigned long i = 0; i < clients_.size() ; i++)
	{
		if (clients_[i] != sender)
		{
			write(clients_[i]->getFdSocket(), out.c_str(), out.size());
			std::cout << out << std::endl;
		}
	}
}
