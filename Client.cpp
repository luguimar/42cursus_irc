#include "Client.hpp"

Client::Client()
{
	_fd = -1;
	_ip = "";
	_nick = "";
    _old_nick = "";
	_user = "";
	_password = "";
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

std::string Client::getOldNick()
{
	return _old_nick;
}

void Client::setOldNick()
{
	_old_nick = _nick;
}

std::string Client::getUser()
{
	return _user;
}

void Client::setUser(std::string user)
{
	_user = user;
}

std::string Client::getPass()
{
	return _password;
}

void Client::setPass(std::string password)
{
	_password = password;
}

bool Client::getAuth()
{
	return _auth;
}

void Client::setAuth(bool auth)
{
	_auth = auth;
}

/*void Client::initClient()
{
	std::cout << "Time to set up your user!" << std::endl;
	std::cout << "Insert nickname: ";
	std::getline(std::cin, this->_nick);
	std::cout << std::endl;
	std::cout << "Insert username: ";
	std::getline(std::cin, this->_user);
	std::cout << std::endl;
	std::cout << "Insert password: ";
	std::getline(std::cin, this->_password);
}*/
