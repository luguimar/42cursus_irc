#include "../Server.hpp"

void Server::privmsg(int fd_c, std::vector <std::string> cmd)
{
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
		return;
	}
	//:irc.underworld.no 402 test asdasd :No such server
	//ERR_CANNOTSENDTOCHAN (404)
	//  "<client> <channel> :Cannot send to channel"
	//RPL_AWAY (301)
	//  "<client> <nick> :<message>"
	//:localhost <number> <nick> :message
	//if client not online atm send RPL_AWAY (301)
	//chanel communication (devera fazer)
	//if channel has some restrictions and cannot send ERR_CANNOTSENDTOCHAN (404)
	std::string message = cmd[2];
	std::string target = cmd[1];

	message += "\r\n";

	if (target[0] == '#' || target[0] == '&') //e um channel
	{
		Channel *chan_target = getChannel(target);
		if (chan_target == NULL)
		{
			error_msg += " 401 " + client->getNick() + " " + target + " :No such nick/channel\r\n";
			send(fd_c, error_msg.c_str(), error_msg.size(), 0);
			return ;
		}
		std::string new_message = ":" + client->getNick() + "!" + client->getUser() + "@localhost PRIVMSG " + target + " :" + message;
		chan_target->broadcast(new_message, fd_c);
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
		std::string new_message = ":" + client->getNick() + "!" + client->getUser() + "@localhost PRIVMSG " + target + " :" + message;
		send(client_target->getFd(), new_message.c_str(), new_message.size(), 0);
	}
	//:hellohell!~helloh@62.48.185.7 PRIVMSG test :hello
	//:<nick_of_sender>!<user_of_sender>@localhost <PRIVMSG> <target> :<message>
}
