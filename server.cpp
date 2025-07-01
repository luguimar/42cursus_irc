#include "irc.hpp"

Server::Server()
{
	_server_port = 0;
	_server_socket_fd = 0;
}

Server::~Server()
{

}

void Server::startSocket()
{
	struct sockaddr_in server_adr;		//struct propria para guardar informacao importate sobre o address do server (normalmente usada para IPv4)
	//struct pollfd name;

	server_adr.sin_family = AF_INET; 	//int que representa o tipo de address do server 'AF_INET' indica que e IPv4
	server_adr.sin_port = htons(_server_port); //16-bit int que guarda a port do server em "network byte order" por isso e que se usa o htons para transformar a mesma
	server_adr.sin_addr.s_addr = INADDR_ANY; //por fim o proprio address do server (IPv4) como nos queremos que se conecte a 'todas' usamos a flag 'INADDR_ANY'

	_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
}

void Server::startServer(char *port)
{
	_server_port = std::atoi(port);
	startSocket();

	std::cout << "Server port: " << _server_port << "\nServer socket(fd): " << _server_socket_fd << std::endl;
}