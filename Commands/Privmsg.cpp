#include "../Server.hpp"

void Server::privmsg(int fd_c, std::vector <std::string> cmd)
{
	std::cout << "Entered Privmsg" << std::endl;

	std::string error_msg = ":localhost";
	Client *client = getClientByFd(fd_c);

	if (cmd.size() < 3)
	{
		if(cmd.size() <= 1)
		{
			error_msg += " 411 " + client->getNick() + " :No recipient given (PRIVMSG)\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size(), 0);
		}
		else
		{
			error_msg += " 412 " + client->getNick() + " :No text to send\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size(), 0);
		}
		return ;
	}
	//:irc.underworld.no 402 test asdasd :No such server
	//ERR_CANNOTSENDTOCHAN (404)
	//  "<client> <channel> :Cannot send to channel"
	//RPL_AWAY (301)
	//  "<client> <nick> :<message>"
	//:localhost <number> <nick> :message
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
	else // e um client
	{
		Client *client_target = getClientByNick(target);
		if (client_target == NULL)
		{
			error_msg += " 401 " + client->getNick() + " " + target + " :No such nick/channel\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size(), 0);
			return ;
		}
		send(client_target->getFd(), message.c_str(), message.size(), 0);
	}
	//:hellohell!~helloh@62.48.185.7 PRIVMSG test :hello
	//:<nick_of_sender>!<user_of_sender>@localhost <PRIVMSG> <target> :<message>
}
