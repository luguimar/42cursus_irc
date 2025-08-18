#include "../Server.hpp"

int	check_nick(const char *nick)
{
	int	i;

	i = -1;
	while (nick[++i] != '\0')
	{
		if (nick[0] == '#' || nick[0] == '&' || nick[0] == ':' || (nick[0] >= '0' && nick[0] <= '9'))
			return (1);
		else if (nick[i] == ' ')
			return (1);
	}
	return (0);
}

void Server::setnick(int fd_c, std::vector<std::string> cmd)
{
	std::cout << "Setting up nickname" << std::endl;
	if (cmd.size() < 2)
		return (void)send(fd_c, "ERR_NONICKNAMEGIVEN (431)\r\n", 29, 0);
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNick() == cmd[1])
			return (void)send(fd_c, "ERR_NICKNAMEINUSE (433)\r\n", 27, 0);
	}
	if (check_nick(cmd[1].c_str()))
		return (void)send(fd_c, "ERR_ERRONEUSNICKNAME (432)\r\n", 30, 0);
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd_c)
		{
			if (_clients[i].getNick() == "")
			{
				std::string msg = "Nickname successfully registered as " + cmd[1] + "\r\n";
				send(fd_c, msg.c_str(), (40 + cmd[1].size()), 0);
				_clients[i].setNick(cmd[1]);
			}
			else
			{
				std::string msg = _clients[i].getNick() + "changed his nickname to " + cmd[1] + "\r\n";
				send(fd_c, msg.c_str(), (_clients[i].getNick().size() + 28 + cmd[i].size()), 0);
			}
			_clients[i].setOldNick();
			_clients[i].setNick(cmd[1]);
			//:Alejandro!Ale_j@localhost NICK hello2
			//:oldname!user@localhost NICK newnick
			//std::string msg2 = ":" + _clients[i].getNick() + "!" + _clients[i].getUser(); + "@localhost " + cmd[0] + " " + cmd[1] + "\r\n";
			//std::cout << "Msg: " << msg2 << std::endl;
		}
	}
	// falta mandar a mensagem de sucesso pro server
}

