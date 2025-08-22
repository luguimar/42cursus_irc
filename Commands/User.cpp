#include "../Server.hpp"

#define USERLEN 30

void Server::setuser(int fd_c, std::vector<std::string> cmd)
{
	std::cout << "Setting up username" << std::endl;

	std::string	realname;
	std::string	username;

	if (cmd.size() < 5 || cmd[1].size() < 1)
		return (void)send(fd_c, "ERR_NEEDMOREPARAMS (461)\r\n", 28, 0); 
	if (cmd[2] != "0" || cmd[3] != "*") //checar se é suposto ser aspas simples ou duplas
		return (void)send(fd_c, "Wrong arguments, try '<username> 0 * :<realname>' instead\r\n", 60, 0);
	if (cmd[1].size() > USERLEN)
		cmd[1] = (cmd[1].substr(0, USERLEN) + ".");
	username = "~" + cmd[1];
	for (size_t i = 4; i < cmd.size(); i++)
		realname = realname + cmd[i];
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd_c)
		{
			if (_clients[i].getAuth())
				return (void)send(fd_c, "ERR_ALREADYREGISTERED (462)\r\n", 31, 0);
			_clients[i].setUser(username);
			_clients[i].setReal(realname);
		}
	}

	//esta parte e a tal autenticacao
	if (getClientByFd(fd_c)->getNick() != "*" && !getClientByFd(fd_c)->getAuth())
	{
		if (getClientByFd(fd_c)->getPass() != _server_pass)
		{
			std::string error_msg = ":localhost 462 " + getClientByFd(fd_c)->getNick() + " :Password incorrect\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size() , 0);
			return ;
		}
		getClientByFd(fd_c)->setAuth(true);
		sendWelcomeBurst(fd_c);
	}
}

