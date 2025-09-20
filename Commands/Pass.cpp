#include "../Server.hpp"

void Server::setpass(int fd_c, std::vector <std::string> cmd)
{
    std::string error_msg = ":localhost";
	Client *client = getClientByFd(fd_c);

	if (cmd.size() < 2)
    {
        error_msg += " 461 " + client->getNick() + " PASS :Not enough arguments\r\n";
        send(fd_c, error_msg.c_str(), error_msg.size() , 0);
		return ;
    }

	if (client->getAuth() || client->getNick() != "*" || client->getUser() != "~default")
	{
		error_msg += " 462 " + client->getNick() + " :You may not reregister\r\n";
		send(fd_c, error_msg.c_str(), error_msg.size() , 0);
		return ;
	}
	else
	    client->setPass(cmd.at(1));
}
