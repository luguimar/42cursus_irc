#ifndef SERVER_HPP
#define SERVER_HPP

//libs
#include <iostream>
#include <ctime>
#include <cctype>
#include <string>
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
#include "Channel.hpp"
#include "Client.hpp"


#define NETWORK_NAME "IrrationalRelayCreation"
#define VERSION "1.7.7"

class Channel; // forward declaration
class Client;  // forward declaration

//server class
class Server
{
	private:
		int	_server_port;		//-> port do server | exemplo: 8080 ( desde que esteja neste intervalo nao dara problemas (em principio) 1024 to 49151)
		int	_server_socket_fd;	//-> fd que o socket devolve
		static bool _server_live;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _fds;
		std::vector<Channel> _channels;
        std::string	_server_pass;
		std::string _start_time;

		Channel *getChannel(const std::string &name);
		Client *getClientByFd(int fd);
		Client *getClientByNick(std::string nick);

	public:
		Server();
		~Server();

		//server
		void	startServer(char *port);
		void	startSocket();
		void	newClient();
		void	receivedData(int id, int fd);
		void	parseExec(int id, int fd_c, std::string buf);

		//ctrl + c etc handeler
		static void SignalHandler(int signum);

		//cmds
		void	join(int fd_c, std::vector<std::string> cmd);
		void	privmsg(int fd_c, std::vector<std::string> cmd);
		void	setnick(int fd_c, std::vector<std::string> cmd);
		void	setpass(int fd_c, std::vector<std::string> cmd);
		void	setuser(int fd_c, std::vector<std::string> cmd);

		//server auth message
		void sendWelcomeBurst(int fd_c);

        //get&setter
        std::string	getServerPass();
        void	setServerPass(std::string pass);
		std::string	getServerStartTime();
		void	setServerStartTime(std::time_t time);

		//cleaners
		void	closeFd();
		void	clearClient(int fd);
};

#endif
