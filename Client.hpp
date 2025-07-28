#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <string>

//_c something of client
class Client
{
private:
	int	_fd;		 //fd do client ou neste caso o fd que representa a socket do cliente ligada so servidor (server.cpp, newClient()) mas sobre
	std::string _ip; //ip do cliente outraves (server.cpp, newClient())

public:
	Client();
	~Client();

	int getFd();
	void setFd(int fd);
	std::string getIp();
	void setIp(std::string ip);
};

#endif