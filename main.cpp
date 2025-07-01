#include "irc.hpp"

int main(int argc, char **argv)
{
	//parsing
	//argv[1] = port
	//argv[2] = password
	(void)argc;
	//testing nothing serious yet!!!
	Server server;

	server.startServer(argv[1]);
}