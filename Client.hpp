#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <string>

//_c something of client
class Client
{
	private:
		int	_fd;		 	//fd do client ou neste caso o fd que representa a socket do cliente ligada so servidor (server.cpp, newClient()) mas sobre
		std::string _ip; 	//ip do cliente outraves (server.cpp, newClient())
		std::string _nick;
		std::string _user;
		std::string _password;
		bool	_auth;
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

		bool	getAuth();
		void setAuth(bool auth);

		// need to check if it is fine and where to put it
		void initClient();
};

#endif
