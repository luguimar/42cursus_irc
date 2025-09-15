#include "../Server.hpp"

int	check_nick(const char *nick)
{
	int	i;

	i = -1;
	while (nick[++i] != '\0')
	{
		if (nick[0] == '#' || nick[0] == '&' || nick[0] == ':' || nick[0] == '*' || (nick[0] >= '0' && nick[0] <= '9'))
			return (1);
		else if (nick[i] == ' ')
			return (1);
	}
	return (0);
}

void Server::setnick(int fd_c, std::vector<std::string> cmd)
{
	std::string	error_msg;
	Client *client = getClientByFd(fd_c);
	std::cout << "Setting up nickname" << std::endl;

	if (cmd.size() < 2)
	{
		error_msg = ":localhost 431 " + client->getNick() + " :No nickname given\r\n";
		return (void)send(fd_c, error_msg.c_str(), error_msg.size(), 0);
	}
	if (client->getNick() == cmd[1])
	{
		error_msg = ":localhost 433 " + client->getNick() + " " + cmd[1] + " :Nickname is already in use\r\n";
		return (void)send(fd_c, error_msg.c_str(), error_msg.size(), 0);
	}
	if (check_nick(cmd[1].c_str()))
	{
		error_msg = ":localhost 432 " + client->getNick() + " " + cmd[1] + " :Erroneus nickname\r\n";
		return (void)send(fd_c, error_msg.c_str(), error_msg.size(), 0);
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd_c)
		{
			if (_clients[i].getNick() != "*")
			{
				std::string msg = ":" + _clients[i].getNick() + "!" + _clients[i].getUser() + "@localhost NICK :" + cmd[1] + "\r\n";
				send(fd_c, msg.c_str(), msg.size(), 0);
			}
			_clients[i].setNick(cmd[1]);
		}
	}
	// falta mandar a mensagem de sucesso pro server

	if (getClientByFd(fd_c)->getUser() != "~default" && !getClientByFd(fd_c)->getAuth())
	{
		if (getClientByFd(fd_c)->getPass() != _server_pass)
		{
			std::string error_msg = ":localhost 464 " + getClientByFd(fd_c)->getNick() + " :Password incorrect\r\n";
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

