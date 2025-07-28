#include "Client.hpp"

Client::Client()
{
	_fd = -1;
	_ip = "";
	_nick = "";
	_user = "";
}

Client::~Client()
{

}

int Client::getFd()
{
	return _fd;
}

void Client::setFd(int fd)
{
	_fd = fd;
}

std::string Client::getIp()
{
	return _ip;
}

void Client::setIp(std::string ip)
{
	_ip = ip;
}

std::string Client::getNick()
{
	return _nick;
}

void Client::setNick(std::string nick)
{
	_nick = nick;
}

std::string Client::getUser()
{
	return _user;
}

void Client::setUser(std::string user)
{
	_user = user;
}