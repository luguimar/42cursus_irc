#include "irc.hpp"

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		Server server;
		server.startServer(argv[1]);
	}
	else
	{
		std::cout << "Usar uma port pelos menos password nao e precisso. exemplo de port 8080" << std::endl;
	}
}