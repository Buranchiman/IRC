#ifndef Channel_HPP
#define Channel_HPP

#include "Client.hpp"
#include <vector>
#include <unistd.h>

class Client;

class Channel
{
private:
	std::string name_;
	std::string topic_;
	std::string password_;
	// bool		topicRestriction_;
	// bool		inviteOnly_;
	std::vector<Client *> clients_;
	std::vector<Client *> operators_;
public:
	Channel();
	Channel(std::string name, std::string topic);
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	~Channel();

	void	join(Client &client);
	void	msgEveryone(Client *sender, std::string msg);
};

#endif /* Channel_HPP */
