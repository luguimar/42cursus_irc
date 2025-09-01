#include "../Server.hpp"

#define USERLEN 30

void Server::setuser(int fd_c, std::vector<std::string> cmd)
{
	std::cout << "Setting up username" << std::endl;

	std::string	error_msg;
	std::string	realname;
	const char	*username;
	Client	*client = getClientByFd(fd_c);

	if (cmd.size() < 5 || cmd[1].size() < 1)
	{
		error_msg = ":localhost 461 " + client->getNick();
		for (size_t i = 0; i < cmd.size(); i++)
			error_msg += " " + cmd[i];
		error_msg += " :Not enough parameters";
		return (void)send(fd_c, error_msg.c_str(), error_msg.size(), 0);
	}
	if ((cmd[2] != "0" && cmd[2] != "*") || (cmd[3] != "0" && cmd[3] != "*")) //checar se é suposto ser aspas simples ou duplas
		return (void)send(fd_c, "Wrong arguments, try '<username> (0 *) or (* 0) :<realname>' instead\r\n", 60, 0);
	if (cmd[1].size() > USERLEN)
		cmd[1] = (cmd[1].substr(0, USERLEN) + ".");
	username = cmd[1].c_str();
	if (username[0] == '~')
		return (void)send(fd_c, "Username can't start with ~\r\n", 31, 0);
	for (size_t i = 4; i < cmd.size(); i++)
		realname = realname + cmd[i];
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd_c)
		{
			if (_clients[i].getAuth())
			{
				error_msg = ":localhost 462 " + client->getNick() + " :You may not reregister\r\n";
				return (void)send(fd_c, error_msg.c_str(), error_msg.size(), 0);
			}
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
			error_msg = "ERROR :Closing Link: localhost (Bad Password)\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size() , 0);
			clearClient(fd_c);
			close(fd_c);
			return ;
		}
		getClientByFd(fd_c)->setAuth(true);
		sendWelcomeBurst(fd_c);
	}
}

