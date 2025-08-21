#include "../Server.hpp"

void Server::setpass(int fd_c, std::vector <std::string> cmd)
{
    std::string error_msg = ":localhost";
	Client *client = getClientByFd(fd_c);

	if (cmd.size() < 2)
    {
        error_msg += " 461 " + client->getNick() + " PASS :Not enough arguments\r\n";
        send(fd_c, error_msg.c_str(), error_msg.size() , 0);
		return ; // error (461)
    }

	if (client->getAuth())
	{
		error_msg += " 462 " + client->getNick() + " :You may not reregister\r\n";
		send(fd_c, error_msg.c_str(), error_msg.size() , 0);
		return ; // error (462)
	}
    else
    	client->setPass(cmd.at(1));
    std::cout << "Server pass: " << getServerPass() << std::endl;
	std::cout << "Client pass: " << client->getPass() << std::endl;
	//testing welcome message
	/*if (getServerPass() == client->getPass())
	{
		client->setAuth(true);
		sendWelcomeBurst(fd_c);
	}*/
}

//erro (461) not enough
//:localhost 461 nick cmd :Not enough parameters
//erro (462) alredy
//:localhost 462 nick :You may not reregister
//erro (464) pass does not match
//:localhost 464 nick :Password incorrect