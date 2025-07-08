#include "irc.hpp"

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		Server server;

		try
		{
			signal(SIGINT, Server::SignalHandler);
			signal(SIGQUIT, Server::SignalHandler);
			server.startServer(argv[1]);
		}
		catch (std::exception &e)
		{
			server.closeFd();
			std::cout << e.what() << std::endl;
		}
	}
	else
	{
		std::cout << "Usar uma port pelos menos password nao e precisso. exemplo de port 8080" << std::endl;
	}
}