#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		std::cout << "Usage: ./ircserver <port> <pass>.\n" << std::endl;
	else
	{
		Server server;

		try
		{
			signal(SIGINT, Server::SignalHandler);
			signal(SIGQUIT, Server::SignalHandler);
			server.setServerPass(argv[2]);
			server.startServer(argv[1]);
		}
		catch (std::exception &e)
		{
			server.closeFd();
			std::cout << e.what() << std::endl;
		}
	}
}