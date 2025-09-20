#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		std::cout << "Usage: ./ircserv <port> <pass>" << std::endl;
	else
	{
		Server server;

		try
		{
			signal(SIGINT, Server::SignalHandler);
			signal(SIGQUIT, Server::SignalHandler);
			signal(SIGPIPE, SIG_IGN);
			server.setServerPass(argv[2]);
			server.setServerStartTime(std::time(NULL));
			server.startServer(argv[1]);
		}
		catch (std::exception &e)
		{
			server.closeFd();
			std::cout << e.what() << std::endl;
		}
	}
}
