#include "Client.hpp"

Client::Client()
{
	_fd = -1;
	_ip = "";
	_nick = "";
	_user = "";
	_password = "";
    _auth = false;
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
	if (_nick == "")
          return "*";
	return _nick;
}

void Client::setNick(std::string nick)
{
	_nick = nick;
}

std::string Client::getUser()
{
	if (_user == "")
		return "~default";
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

std::string Client::getBuf()
{
	return _buf;
}

void Client::setBuf(std::string buf)
{
	_buf = buf;
}

bool Client::getAuth()
{
	return _auth;
}

void Client::setAuth(bool auth)
{
	_auth = auth;
}
