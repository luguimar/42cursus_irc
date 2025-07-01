#ifndef _IRC_HPP
#define _IRC_HPP

//libs
#include <iostream>
#include <string>
#include <cstdlib>		//atoi()
#include <sys/socket.h> //socket()
#include <netinet/in.h> //sockaddr_in struct
#include <poll.h>		//poll() & pollfd struct

//server class
class Server
{
	private:
		int	_server_port;		//-> port do server | exemplo: 8080 ( desde que esteja neste intervalo nao dara problemas (em principio) 1024 to 49151)
		int	_server_socket_fd;	//-> fd que o socket devolve

	public:
		Server();
		~Server();

		void	startServer(char *port);
		void	startSocket();
};

//client class
/*class Client
{

};*/

#endif