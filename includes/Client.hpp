#pragma once
#include <iostream>
#include <string>
#include <Channel.hpp>
#include <stdio.h>
#include <sys/socket.h>

class Channel;

class Client
{
private:
	std::string userName_;
	std::string nickName_;
	// std::string passWord_;
	std::string	pendingInput;
	bool		hasUsername;
	bool		hasNickname;
	bool		hasWelcomeSent;
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
	void setNickName(std::string nickName);
	void setReading(bool opt);
	void reset();
	void initialize(int fdSocket, const char *userName);
	void setChannel(Channel *channel);
	void setWelcomeSent(bool status);
	//getter
	std::string getUserName() const;
	std::string getNickName() const;
	int getFdSocket() const;
	bool getNameStatus() const;
	bool getNicknameStatus() const;
	bool getWelcomeSentStatus() const;
	std::string getInput() const;
	Channel		*getChannel() const;

	std::string	&accessBuffer();
	static Client	**createPool(int maxClients);
	static void		destroyPool(Client **clients, int maxClients);
	void			writeOnTerm(std::string message);
};

void trim(std::string &str);

void send_all(int fd, const std::string &msg);
