#include "../Server.hpp"

void Server::join(int fd_c, std::vector<std::string> cmd)
{
	std::cout << "Entered Join" << std::endl;
	Client *client = getClientByFd(fd_c);

	if (cmd.size() < 2)                               // 461 ERR_NEEDMOREPARAMS
    {
		std::string error_msg = ":localhost 461 " + client->getNick() + " JOIN :Not enough arguments\r\n";
		send(fd_c, error_msg.c_str(), error_msg.size() , 0);
        return ;
    }
	/* separar vários canais */
	std::istringstream ss(cmd[1]);
	std::string chanName;

	while (std::getline(ss, chanName, ','))
	{
		if (chanName.empty() || (chanName[0] != '#' && chanName[0] != '&'))
		{
			std::string error_msg = ":localhost 476 " + client->getNick() + " " + chanName + " :Bad Channel Mask\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size() , 0);
			return ;
		}

		Channel *chan = getChannel(chanName);
		if (!chan)                                    // se não existir, cria‑se
		{
			_channels.push_back(Channel(chanName));
			chan = &_channels.back();
		}

		if (!chan->hasMember(fd_c)) {
			chan->addMember(fd_c);

			if (chan->getMembers().size() == 1) // ← primeiro membro
				chan->makeOperator(fd_c);       // ← operador do canal
		}
		else
			continue;	// 443 ERR_USERONCHANNEL

		/* Mensagens IRC mínimas: JOIN e NAMES */
		std::string joinMsg = ":localhost JOIN " + chanName + "\r\n";
		chan->broadcast(joinMsg, -1);                 // avisa toda a gente

		/* 353 RPL_NAMREPLY */
		std::string names = "= " + chanName + " :";
		for (std::set<int>::iterator it = chan->getMembers().begin();
			 it != chan->getMembers().end(); ++it)
		{
			Client *cl = getClientByFd(*it);
			names += cl ? cl->getIp() + " " : "";     // ou usa nick se já tiveres
		}
		names += "\r\n";
		send(fd_c, ("353 " + names).c_str(), names.length() + 4, 0);
		/* 366 RPL_ENDOFNAMES */
		send(fd_c, ("366 " + chanName + " :End of /NAMES list\r\n").c_str(),
			 chanName.length() + 30, 0);
	}

}
