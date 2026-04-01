#pragma once
#include <iostream>

class Client
{
private:
	std::string userName_;
	// std::string nickName_;
	// std::string passWord_;
	int fdSocket_;
public:
	Client();
	~Client();

	//setter
	void setFdSocket(int fdSocket);
	void setUserName(char *userName);
	//getter
	std::string getUserName();
	int getFdSocket() const;

};

