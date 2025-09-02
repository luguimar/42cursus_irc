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

		/*
		//Mensagens IRC mínimas: JOIN e NAMES
		std::string joinMsg = ":localhost JOIN " + chanName + "\r\n";
		chan->broadcast(joinMsg, -1);                 // avisa toda a gente

		//353 RPL_NAMREPLY
		std::string names = "= " + chanName + " :";
		for (std::set<int>::iterator it = chan->getMembers().begin();
			 it != chan->getMembers().end(); ++it)
		{
			Client *cl = getClientByFd(*it);
			names += cl ? cl->getIp() + " " : "";     // ou usa nick se já tiveres
		}
		names += "\r\n";
		send(fd_c, ("353 " + names).c_str(), names.length() + 4, 0);
		//366 RPL_ENDOFNAMES
		send(fd_c, ("366 " + chanName + " :End of /NAMES list\r\n").c_str(),
			 chanName.length() + 30, 0);
		*/
		std::string new_message = ":" + client->getNick() + "!" + client->getUser() + "@localhost JOIN :" + chanName + "\r\n";
		send(fd_c, new_message.c_str(), new_message.size() , 0);
		//if para a mensagem embaixo que falta
		new_message = ":localhost 332" + client->getNick() + " " + chanName + ":Topic\r\n";
		send(fd_c, new_message.c_str(), new_message.size() , 0);
		//uma forma de conseguir o prefix dos membros a lista dos mesmo membros e mudar o igual para o que esta embaixo quando conseguir fazer o memsmo
		new_message = ":localhost 353" + client->getNick() + " = " + chanName + ":Clients\r\n";
		send(fd_c, new_message.c_str(), new_message.size() , 0);
		new_message = ":localhost 366" + client->getNick() + " " + chanName + ":End of /NAMES list\r\n";
		send(fd_c, new_message.c_str(), new_message.size() , 0);
		//:nick!user@localhost JOIN :channelname
		//does channel have topic?
		//:localhost 332 nick_c #channelname :"Topic"
		//:localhost 353 nick_c =/* #channelname :alice @dan
		//if channels is setted to inv its * if not = | for member only add @ if its an operator and always first user joining and then list
		//:localhost 366 nick_c #channelname :End of /NAMES list
	}
}
