#pragma once
#include <iostream>
#include <string>
#include <Channel.hpp>

class Channel;

class Client
{
private:
	std::string userName_;
	// std::string nickName_;
	// std::string passWord_;
	std::string	pendingInput;
	bool		hasUsername;
	int 		fdSocket_;
	Channel		*channel_;
public:
	Client();
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client();

	//setter
	void setFdSocket(int fdSocket);
	void setUserName(std::string userName);
	void setReading(bool opt);
	void reset();
	void initialize(int fdSocket, const char *userName);
	void setChannel(Channel *channel);
	//getter
	std::string getUserName() const;
	int getFdSocket() const;
	bool getNameStatus() const;
	std::string getInput() const;
	Channel		*getChannel() const;

	std::string	&accessBuffer();
	static Client	**createPool(int maxClients);
	static void		destroyPool(Client **clients, int maxClients);
	void			writeOnTerm(std::string message);
};

void trim(std::string &str);
