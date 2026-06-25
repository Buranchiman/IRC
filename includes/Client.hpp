#pragma once
#include <iostream>
#include <string>
#include <vector>
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
	std::string	pendingInput_;
	bool		hasUsername_;
	bool		hasNickname_;
	bool		hasCapStart_;
	bool		hasCapEnd_;
	bool		welcomeSent_;
	bool		registered_;
	int 		fdSocket_;
	std::vector<Channel *>		channels_;
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
	void addChannel(Channel *channel);
	void suppChannel(Channel *channel);

	// void setChannel(Channel *channel);
	void setwelcomeSent_(bool status);
	void setCapStart(bool status);
	void setCapEnd(bool status);
	void setregistered_(bool status);
	//getter
	std::string getUserName() const;
	std::string getNickName() const;
	int getFdSocket() const;
	bool getNameStatus() const;
	bool getNicknameStatus() const;
	bool getwelcomeSent_Status() const;
	bool gethasCapStart_() const;
	bool gethasCapEnd_() const;
	bool getregistered_() const;
	std::string getInput() const;
	std::vector<Channel *> &getChannels();

	std::string	&accessBuffer();
	static Client	**createPool(int maxClients);
	static void		destroyPool(Client **clients, int maxClients);
	void			writeOnTerm(std::string message);
	void			checkRegistration(void);
};

Client* findClientByNickname(const std::vector<Client *> &clients, const std::string &nickname);

void trim(std::string &str);

void send_all(int fd, const std::string &msg);
