#include "../Server.hpp"

void Server::privmsg(int fd_c, std::vector <std::string> cmd)
{
	std::cout << "Entered Privmsg" << std::endl;
	if (cmd.size() < 3)
	{
		if(cmd.size() <= 1)
			send(fd_c, "ERR_NORECIPIENT (411)\r\n", 25, 0);
		else
			send(fd_c, "ERR_NOTEXTTOSEND (412)\r\n", 26, 0);
		return ;
	}
	//send(_clients[1].getFd(), cmd[2].c_str(), cmd[2].size(), 0);
	//if its client  look for no client print error ERR_NOSUCHNICK (401) if no server ERR_NOSUCHSERVER (402)
	//client part (neck nicknames so will not do rn)
	//if client not online atm send RPL_AWAY (301)
	//chanel communication (devera fazer)
	//if channel has some restrictions and cannot send ERR_CANNOTSENDTOCHAN (404)
	std::string message;
	std::string target = cmd[1];

	for (size_t i = 2; i < cmd.size(); i++)
	{
		message += cmd[i];
		if (i + 1 != cmd.size())
			message += " ";
	}

	message += "\r\n";

	if (target[0] == '#' || target[0] == '&') //e um channel
	{
		Channel *chan_target = getChannel(target);
		chan_target->broadcast(message, fd_c);
	}
	//else // e um client
}
