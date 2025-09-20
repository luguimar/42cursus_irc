#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include <string>

class Client
{
	private:
		int	_fd;
		std::string _ip;
		std::string _nick;
		std::string _user;
		std::string _real;
		std::string _password;
		std::string _buf;
		std::string _buf_saver;
		bool	_auth;

		time_t      _lastActivity;
		bool        _awaitingPong;
		std::string _lastPingToken;

	public:
		Client();
		~Client();

		int getFd();
		void setFd(int fd);

		std::string getIp();
		void setIp(std::string ip);

		std::string getNick();
		void setNick(std::string nick);

		std::string getUser();
		void setUser(std::string user);

		std::string getPass();
		void setPass(std::string password);

		std::string getBuf();
		void setBuf(std::string buf);

		bool	getAuth();
		void setAuth(bool auth);

		std::string getReal();
		void setReal(std::string real);

		std::string getBufSaver();
		void setBufSaver(std::string buf, bool flag);

		time_t      getLastActivity() const;
		void        touch();
		bool        awaitingPong() const;
		void        setAwaitingPong(bool v);
		const std::string& lastPingToken() const;
		void        setLastPingToken(const std::string& t);
};

#endif
