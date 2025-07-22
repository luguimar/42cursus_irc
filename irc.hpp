#ifndef _IRC_HPP
#define _IRC_HPP

//libs

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <bits/stdc++.h>

//client class
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

//_c something of client
//server class
class Server
{
	private:
		int	_server_port;		//-> port do server | exemplo: 8080 ( desde que esteja neste intervalo nao dara problemas (em principio) 1024 to 49151)
		int	_server_socket_fd;	//-> fd que o socket devolve
		static bool _server_live;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _fds;

	public:
		Server();
		~Server();

		void	startServer(char *port);
		void	startSocket();
		void	newClient();
		void	receivedData(int fd);
		void	parseExec(int fd_c, std::string buf);

		static void SignalHandler(int signum);

		void	closeFd();
		void	clearClient(int fd);
};

#endif