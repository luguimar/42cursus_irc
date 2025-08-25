#include "Client.hpp"

Client::Client()
{
	_fd = -1;
	_ip = "";
	_nick = "";
	_user = "";
	_real = "";
	_password = "";
    _auth = false;
}

Client::~Client()
{

}

int Client::getFd() const
{
	return _fd;
}

void Client::setFd(const int fd)
{
	_fd = fd;
}

std::string Client::getIp()
{
	return _ip;
}

void Client::setIp(const std::string& ip)
{
	_ip = ip;
}

std::string Client::getOldNick()
{
	return _old_nick;
}

void Client::setOldNick()
{
	_old_nick = _nick;
}

std::string Client::getNick()
{
	if (_nick.empty())
          return "*";
	return _nick;
}

void Client::setNick(const std::string& nick)
{
	_nick = nick;
}

std::string Client::getUser()
{
	if (_user.empty())
		return "~default";
	return _user;
}

void Client::setUser(const std::string& user)
{
	_user = user;
}

std::string Client::getPass()
{
	return _password;
}

void Client::setPass(const std::string& password)
{
	_password = password;
}

std::string Client::getBuf()
{
	return _buf;
}

void Client::setBuf(const std::string& buf)
{
	_buf = buf;
}

bool Client::getAuth() const
{
	return _auth;
}

void Client::setAuth(const bool auth)
{
	_auth = auth;
}

std::string Client::getReal()
{
	return _real;
}

void Client::setReal(const std::string& real)
{
	_real = real;
}
