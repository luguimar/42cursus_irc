#include "../Server.hpp"

void Server::join(int fd_c, std::vector<std::string> cmd)
{
    std::cout << "Entered Join" << std::endl;
    Client *client = getClientByFd(fd_c);
    if (!client)
        return;

    // 461 ERR_NEEDMOREPARAMS
    if (cmd.size() < 2) {
        std::string error_msg = ":localhost 461 " + client->getNick() + " JOIN :Not enough parameters\r\n";
        send(fd_c, error_msg.c_str(), error_msg.size(), 0);
        return;
    }

    // Parse da lista de canais em cmd[1] (ex.: "#a,#b")
    std::vector<std::string> channels;
    {
        std::istringstream ss(cmd[1]);
        std::string name;
        while (std::getline(ss, name, ',')) {
            channels.push_back(name);
        }
    }

    // Parse opcional da lista de keys em cmd[2] (ex.: "keyA,keyB")
    std::vector<std::string> keys;
    if (cmd.size() >= 3) {
        std::istringstream ks(cmd[2]);
        std::string key;
        while (std::getline(ks, key, ',')) {
            keys.push_back(key);
        }
    }

    // Itera canais mantendo o índice para casar keys[i] (se existir)
    for (size_t i = 0; i < channels.size(); ++i) {
        const std::string chanName = channels[i];
        const std::string providedKey = (i < keys.size()) ? keys[i] : "";

        // 476 ERR_BADCHANMASK
        if (chanName.empty() || (chanName[0] != '#' && chanName[0] != '&')) {
            std::string msg = ":localhost 476 " + client->getNick() + " " + chanName + " :Bad Channel Mask\r\n";
            send(fd_c, msg.c_str(), msg.size(), 0);
            continue;
        }

        // obter/criar canal
        Channel *chan = getChannel(chanName);
        if (!chan) {
            _channels.push_back(Channel(chanName));
            chan = &_channels.back();
        }

        // 443 ERR_USERONCHANNEL (já está no canal)
        if (chan->hasMember(fd_c)) {
            std::string msg = ":localhost 443 " + client->getNick() + " " + client->getNick() + " " + chanName + " :is already on channel\r\n";
            send(fd_c, msg.c_str(), msg.size(), 0);
            continue;
        }

        // --- REGRAS DE ENTRADA: +l, +i, +k ---
        // 471 ERR_CHANNELISFULL (+l)
        if (chan->isFull()) {
            std::string msg = ":localhost 471 " + client->getNick() + " " + chanName + " :Channel is full\r\n";
            send(fd_c, msg.c_str(), msg.size(), 0);
            continue;
        }

        // 473 ERR_INVITEONLYCHAN (+i) se não estiver convidado
        if (chan->isInviteOnly() && !chan->isInvited(client->getNick())) {
            std::string msg = ":localhost 473 " + client->getNick() + " " + chanName + " :Cannot join channel (+i)\r\n";
            send(fd_c, msg.c_str(), msg.size(), 0);
            continue;
        }

        // 475 ERR_BADCHANNELKEY (+k) — convite NÃO dispensa key
        if (chan->hasKey() && chan->getKey() != providedKey) {
            std::string msg = ":localhost 475 " + client->getNick() + " " + chanName + " :Cannot join channel (+k)\r\n";
            send(fd_c, msg.c_str(), msg.size(), 0);
            continue;
        }

        // --- ENTRA NO CANAL ---
        chan->addMember(fd_c);

        // Primeiro membro torna-se operador
        if (chan->memberCount() == 1)
            chan->makeOperator(fd_c);

        // Se estava convidado, limpa o convite
        chan->clearInvite(client->getNick());

        // Prefixo do utilizador (se não tiveres user/host reais, usa placeholders)
        const std::string prefix = ":" + client->getNick() + "!" + client->getUser() + "@localhost ";

        // Broadcast do JOIN (inclui o próprio; é aceitável)
        {
            std::string joinMsg = prefix + "JOIN :" + chanName + "\r\n";
            chan->broadcast(joinMsg, -1);
        }

		//		//new_message = ":localhost 332" + client->getNick() + " " + chanName + ":Topic\r\n";
		//send(fd_c, new_message.c_str(), new_message.size() , 0);
		//:localhost 332 nick_c #channelname :"Topic"

		// 332
		{
			if (!chan->getTopic().empty())
			{
				std::string message = ":localhost 332 " + client->getNick() + " " + chanName + " :" + chan->getTopic() + "\r\n";
				send(fd_c, message.c_str(), message.size() , 0);
			}
		}

        // 353 RPL_NAMREPLY (enviado ao cliente que entrou)
        {
            std::string names = ":localhost 353 " + client->getNick() + " = " + chanName + " :";
            const std::set<int> &m = chan->getMembers();
            for (std::set<int>::const_iterator it = m.begin(); it != m.end(); ++it)
			{
                Client *cl = getClientByFd(*it);
                if (!cl) continue;
                if (chan->isOperator(*it)) names += "@";
                names += cl->getNick();
                names += " ";
            }
            names += "\r\n";
            send(fd_c, names.c_str(), names.size(), 0);
        }

        // 366 RPL_ENDOFNAMES
        {
            std::string endNames = ":localhost 366 " + client->getNick() + " " + chanName + " :End of /NAMES list\r\n";
            send(fd_c, endNames.c_str(), endNames.size(), 0);
        }
    }
}
