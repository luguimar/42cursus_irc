#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

//_c something of client
class Client
{
	private:
		int	_fd;		 	//fd do client ou neste caso o fd que representa a socket do cliente ligada so servidor (server.cpp, newClient()) mas sobre
		std::string _ip; 	//ip do cliente outraves (server.cpp, newClient())
		std::string _nick;
		std::string _old_nick;
		std::string _user;
		std::string _real;
		std::string _password;
		std::string _buf;
		bool	_auth;

	public:
		Client();
		~Client();

		int getFd() const;
		void setFd(int fd);

		std::string getIp();
		void setIp(const std::string& ip);

		std::string getOldNick();
		void setOldNick();

		std::string getNick();
		void setNick(const std::string& nick);

		std::string getUser();
		void setUser(const std::string& user);

		std::string getPass();
		void setPass(const std::string& password);

		std::string getBuf();
		void setBuf(const std::string& buf);

		bool	getAuth() const;
		void setAuth(bool auth);

		std::string getReal();
		void setReal(const std::string& real);
};

#endif
