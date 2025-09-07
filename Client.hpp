#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include <string>

//_c something of client
class Client
{
	private:
		int	_fd;		 	//fd do client ou neste caso o fd que representa a socket do cliente ligada so servidor (server.cpp, newClient()) mas sobre
		std::string _ip; 	//ip do cliente outraves (server.cpp, newClient())
		std::string _nick;
		std::string _user;
		std::string _real;
		std::string _password;
		std::string _buf;
		std::string _buf_saver;
		bool	_auth;

		time_t      _lastActivity;  // atualiza em QUALQUER mensagem válida
		bool        _awaitingPong;  // true quando o server manda PING
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
		//bool, true = add,  false = reset

	    time_t      getLastActivity() const;
	    void        touch(); // atualiza para now
	    bool        awaitingPong() const;
	    void        setAwaitingPong(bool v);
	    const std::string& lastPingToken() const;
	    void        setLastPingToken(const std::string& t);

};

#endif
