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
	_lastActivity = std::time(NULL);
	_awaitingPong = false;
	_lastPingToken.clear();
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

std::string Client::getReal()
{
	return _real;
}

void Client::setReal(std::string real)
{
	_real = real;
}


std::string Client::getBufSaver()
{
	return _buf_saver;
}

void Client::setBufSaver(std::string buf, bool flag)
{
	if (flag)
		_buf_saver += buf;
	else
		_buf_saver = buf;
}

time_t Client::getLastActivity() const { return _lastActivity; }
void   Client::touch() { _lastActivity = std::time(NULL); }
bool   Client::awaitingPong() const { return _awaitingPong; }
void   Client::setAwaitingPong(bool v){ _awaitingPong = v; }
const std::string& Client::lastPingToken() const { return _lastPingToken; }
void   Client::setLastPingToken(const std::string& t){ _lastPingToken = t; }

