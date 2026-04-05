#pragma once
#include <iostream>
#include <string>

class Client
{
private:
	std::string userName_;
	// std::string nickName_;
	// std::string passWord_;
	int fdSocket_;
public:
	Client();
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client();

	//setter
	void setFdSocket(int fdSocket);
	void setUserName(const char *userName);
	void reset();
	void initialize(int fdSocket, const char *userName);
	//getter
	std::string getUserName();
	int getFdSocket() const;

	static Client	**createPool(int maxClients);
	static void		destroyPool(Client **clients, int maxClients);

};

