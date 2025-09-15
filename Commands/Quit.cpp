#include "../Server.hpp"

void Server::quit(int fd_c, std::string message)
{
    std::cout << "ENTROU" << std::endl;
    Client *client = getClientByFd(fd_c);
    std::string quit_message = ":" + client->getNick() + "!" + client->getUser() + "@localhost QUIT :";

    if (message == "")
        quit_message += "Client did not leave a message : ( .\r\n";
    else
        quit_message += message;

    for (int i = 0; i != static_cast<int>(_channels.size()) ; i++)
    {
        if (_channels[i].hasMember(fd_c))
        {
            _channels[i].broadcast(quit_message, fd_c);
            _channels[i].removeMember(fd_c);
        }
    }
}
