#include "../irc.hpp"

void Server::join(int fd_c, std::vector <std::string> cmd)
{
	(void)fd_c;
	(void)cmd;
	int index_c;

	index_c = 0;
	while (_clients[index_c].getFd() != fd_c)
		index_c++;
	std::cout << "Client ip: " << _clients[index_c].getIp() << std::endl;
}