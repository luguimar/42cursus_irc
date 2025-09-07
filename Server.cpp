#include "Server.hpp"

//*-----------[YOU GET IT]-----------*//
Server::Server()
{
	_server_port = -1;
	_server_socket_fd = -1;
	_fds.clear();
	_clients.clear();
}
bool Server::_server_live = false;

Server::~Server()
{

}

//*-----------[GETS & SETS STUFF]-----------*//
Channel *Server::getChannel(const std::string &name)
{
    for (size_t i = 0; i < _channels.size(); ++i)
        if (_channels[i].getName() == name)
            return &_channels[i];
    return NULL;
}

Client *Server::getClientByFd(int fd)
{
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i].getFd() == fd)
            return &_clients[i];
    return NULL;
}

Client *Server::getClientByNick(std::string Nick)
{
	for (size_t i = 0; i < _clients.size(); ++i)
		if (_clients[i].getNick() == Nick)
			return &_clients[i];
	return NULL;
}

std::string Server::getServerPass()
{
	return _server_pass;
}

void Server::setServerPass(std::string pass)
{
	_server_pass = pass;
}

std::string Server::getServerStartTime()
{
	return _start_time;
}

void Server::setServerStartTime(std::time_t time)
{
	_start_time = std::asctime(std::localtime(&time));
}

//*-----------[SIG HANDLER]-----------*//
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "Closing server." << std::endl;
	_server_live = true;
}

//*-----------[SERVER STUFF]-----------*//
void Server::startServer(char *port)
{
	for (size_t i = 0; port[i]; i++)
		if (isalnum(port[i]) == 0)
			throw(std::runtime_error("Please try to use only numbers for the port."));

	_server_port = std::atoi(port);

	startSocket();

	std::cout << "Server port: " << _server_port << "\nServer socket(fd): " << _server_socket_fd << std::endl;

	while (Server::_server_live == false)
	{

		if (poll(_fds.data(), _fds.size(), -1) == -1 && Server::_server_live == false)
			throw(std::runtime_error("The machine heart stopped."));

		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN) {
			    if (_fds[i].fd == _server_socket_fd) newClient();
			    else {
			        receivedData(i, _fds[i].fd);
			        // processa imediatamente após receber (sem depender de POLLOUT)
			        Client* c = getClientByFd(_fds[i].fd);
			        if (c)
			        {
				        parseExec(i, _fds[i].fd, c->getBuf());
			        	c->setBuf("");
			        }
				}
			}
		}
	}
	closeFd();
}

void Server::startSocket()
{
	int enable = 1;
	struct sockaddr_in server_adr;		//struct propria para guardar informacao importate sobre o address do server (normalmente usada para IPv4 (address))
	struct pollfd new_poll;				//struct propria para ser usada com a funcao poll() que ira ajudar a trabalhar com varios fds ao mesmo tempo e tambem tem como uso vigiar os fds guardados

	server_adr.sin_family = AF_INET; 					 //int que representa o tipo de address do server 'AF_INET' indica que e IPv4
	server_adr.sin_port = htons(_server_port); //16-bit int que guarda a port do server em "network byte order" por isso e que se usa o htons para transformar a mesma
	server_adr.sin_addr.s_addr = INADDR_ANY; 			 //por fim o proprio address do server como nos queremos que se conecte a 'todas' usamos a flag 'INADDR_ANY'

	_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	//socket devolve um fd para interargir com a mesma, o primeiro campo e o tipo de address

	if (_server_socket_fd == -1)
		throw(std::runtime_error("Problem when trying to create the socket."));

	//todas as funcoes poddem dar erro que da return de -1 em caso de erro

	if (setsockopt(_server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		throw(std::runtime_error("Problem when trying to set the socket."));
	//setsockopt: funcao usada com o objetivo de alterar "regras/opcoes" da socket

	if (fcntl(_server_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("Problem when trying to set status flags of the socket as \"O_NONBLOCK\"."));
	//fcntl: executa operacoes da propria funcao no fd dado (o fd da socket nest caso)

	if (bind(_server_socket_fd, (struct sockaddr *)&server_adr, sizeof(server_adr)) == -1)
		throw(std::runtime_error("Problem when trying to bind the socket."));
	//bind: serve para atribuir um address e port a uma socket neste caso vamos utilizar o que guardamos em cima

	//quick aula!!!! a socket pode ser tanto passiva como ativa, se for passiva significa que recebe informacao se for ativa significa que manda informacao (epxlicando de uma forma muito splistica)
	//este tipo de "conexao" da-se o nome "TCP/IP connection" (pesquisar um pouco mais profundo nao vou estar a escrever aqui um testamento)
	//como um pouco intuitivo o server e que vai ser a passive socket e o client e que vai ser a active socket, para fazer o server "passivo" usamos a seguinte funcao listen()

	if (listen(_server_socket_fd, SOMAXCONN) == -1)
		throw(std::runtime_error("Problem when trying to make socket become a passive socket."));
	//listen: como explicado assima listen e usado uma funcao que ja esta "bindada" (bind()) para fazer a socket ficar passive

	new_poll.fd = _server_socket_fd;	//fd da socket
	new_poll.events = POLLIN;			//event a fazer neste caso vais ser o POLLIN que e para ler data/informacao
	new_poll.revents = 0;				//historico dos eventos (pelo que percebi) 0 por que ainda nao fizemos nenhum event
	_fds.push_back(new_poll);			//guardar novo poll para ser vigiado pela funcao poll()
}


void Server::newClient()
{
	Client client;
	struct sockaddr_in client_adr;
	struct pollfd new_poll;

	socklen_t len = sizeof(client_adr);

	int client_fd = accept(_server_socket_fd, (sockaddr *)&client_adr, &len);
	//a funcao accept tem como objetivo de estabelecer uma conexao com a socket expecificada no 1* campo devolvendo um fd de outra socket que seria a socket o client

	if (client_fd == -1)
	{
		std::cout << "While trying to make a stable connection it crashed" << std::endl;
		return;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) //ja esta explicado em cima a razao de fazermos isto e a mesma logica (linha 43, server.cpp)
	{
		std::cout << "Problem when trying to set status flags of the socket as \"O_NONBLOCK\". (for client_fd)" << std::endl;
		return;
	}

	new_poll.fd = client_fd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;
	//mesma coisa no de cima por isso nao tem razao de se explicar (linha 67, server.cpp)

	client.setFd(client_fd);							//guarar o fd do client no fd do client da esrutura
	client.setIp(inet_ntoa((client_adr.sin_addr)));	//guardar o ip na estrutura do client (a funcao inet_ntoa() pega num adress/ip e transforma num string e devolve)
	_clients.push_back(client);							//guardar novo client no vector dos clients
	_fds.push_back(new_poll);							//guardar novo poll para vigiar
	std::cout << "New Client" << std::endl;
}

void Server::receivedData(int id, int fd)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	ssize_t data_bytes = recv(fd, buf, sizeof(buf) - 1, 0);

	if (data_bytes <= 0)
	{
		//aka disconnected client
		std::cout << "Something happened to client." << std::endl;
		clearClient(fd);
		close(fd);
	}
	else
	{
		buf[data_bytes] = '\0';
      	Client *client = getClientByFd(fd);
		client->setBuf(buf);
	}
	_fds[id].events = POLLIN;
}

bool verify_token(std::string *token)
{
	for (int i = 0; i < static_cast<int>(token->size()); i++)
    {
		if (token->at(i) == 13)
        {
                  token->erase(token->begin() + i);
                  return true;
        }
    }
	return true;
}

void Server::parseExec(int id, int fd_c, std::string buf)
{
	std::string	token_value;
	std::istringstream buff(buf);
	std::vector<std::string> tokens_by_n;
	std::vector<std::string> tokens;

	_fds[id].events = POLLIN;

	while(std::getline(buff, token_value, '\n'))
		tokens_by_n.push_back(token_value);

	for (int i = 0; i != static_cast<int>(tokens_by_n.size()) ; i++)
	{
		std::istringstream token_buf(tokens_by_n[i]);

		while (std::getline(token_buf, token_value, ' '))
		{
			if (token_value[0] == ':')
			{
				std::string new_token = token_value;
				while (std::getline(token_buf, token_value, ' '))
					new_token += ' ' + token_value;
				new_token.erase(0, 1);
				if (verify_token(&token_value))
					tokens.push_back(new_token);
				break ;
			}
			else
			{
				if (verify_token(&token_value))
					tokens.push_back(token_value);
			}
		}

		if (!tokens.empty())
		{
			for (int i = 0; i != static_cast<int>(tokens[0].size()); i++)
				tokens[0][i] = std::toupper(tokens[0][i]);

			for (int i = 0; i != static_cast<int>(tokens.size()); i++) //printing
				std::cout << "Token[" << i << "]: |" << tokens[i] << "|\r\n";

			if (tokens[0] == "JOIN")
				join(fd_c, tokens);
			else if (tokens[0] == "PRIVMSG")
				privmsg(fd_c, tokens);
			else if (tokens[0] == "NICK")
				setnick(fd_c, tokens);
			else if (tokens[0] == "PASS")
				setpass(fd_c, tokens);
			else if (tokens[0] == "USER")
				setuser(fd_c, tokens);
			else if (tokens[0] == "MODE")       mode(fd_c, tokens);
			else if (tokens[0] == "INVITE")     invite(fd_c, tokens);
			else if (tokens[0] == "KICK")       kick(fd_c, tokens);
			else if (tokens[0] == "TOPIC")      topic(fd_c, tokens);
			// opcional:
			// else if (tokens[0] == "PART")    part(fd_c, tokens);
			// else if (tokens[0] == "NOTICE")  notice(fd_c, tokens);
			else
				std::cout << "Cmd not found." << std::endl;
			tokens.clear();
		}
	}
}

void Server::sendWelcomeBurst(int fd_c)
{
	Client *client = getClientByFd(fd_c);

	std::string msg = ":localhost 001 " + client->getNick() + " :Welcome to the " + NETWORK_NAME + "Network, " + client->getNick() + "\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg = ":localhost 002 " + client->getNick() + " :Your host is localhost, running version " + VERSION + "\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg = ":localhost 003 " + client->getNick() + " :This server was created " + getServerStartTime();
	if (!msg.empty() && msg[msg.size()-1] != '\n') msg += "\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg = ":localhost 004 " + client->getNick() + " localhost " + VERSION + " itkol\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg = ":localhost 005 " + client->getNick() + " CHANMODES=i,t,k,o,l :are supported by this server\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg = ":localhost 375 " + client->getNick() + " :- localhost Message of the day -\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg =	":localhost 372 " + client->getNick() + " :- '##:::::'##:'########:'##::::::::'######:::'#######::'##::::'##:'########: -\r\n"
			":localhost 372 " + client->getNick() + " :-  ##:'##: ##: ##.....:: ##:::::::'##... ##:'##.... ##: ###::'###: ##.....:: -\r\n"
			":localhost 372 " + client->getNick() + " :-  ##: ##: ##: ##::::::: ##::::::: ##:::..:: ##:::: ##: ####'####: ##::::::: -\r\n"
			":localhost 372 " + client->getNick() + " :-  ##: ##: ##: ######::: ##::::::: ##::::::: ##:::: ##: ## ### ##: ######::: -\r\n"
			":localhost 372 " + client->getNick() + " :-  ##: ##: ##: ##...:::: ##::::::: ##::::::: ##:::: ##: ##. #: ##: ##...:::: -\r\n"
			":localhost 372 " + client->getNick() + " :-  ##: ##: ##: ##::::::: ##::::::: ##::: ##: ##:::: ##: ##:.:: ##: ##::::::: -\r\n"
			":localhost 372 " + client->getNick() + " :- . ###. ###:: ########: ########:. ######::. #######:: ##:::: ##: ########: -\r\n"
			":localhost 372 " + client->getNick() + " :- :...::...:::........::........:::......::::.......:::..:::::..::........:: -\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
	msg = ":localhost 376 " + client->getNick() + " :End of /MOTD command.\r\n";
	send(fd_c, msg.c_str(), msg.size(), 0);
}

//*-----------[FREE STUFF]-----------*//
void Server::closeFd()
{
	for (size_t i = 0; i < _clients.size() ; i++)
		close(_clients[i].getFd());
	if (_server_socket_fd != -1)
		close(_server_socket_fd);
}

void Server::clearClient(int fd)
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break ;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break ;
		}
	}
}

int Server::fdByNick(const std::string& nick) {
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i].getNick() == nick) return _clients[i].getFd();
    return -1;
}

std::string Server::nickByFd(int fd) {
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i].getFd() == fd) return _clients[i].getNick();
    return "*";
}

std::string Server::userPrefix(int fd) {
    // se não tiveres user real, usa placeholder
    Client* c = NULL;
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i].getFd() == fd) { c = &_clients[i]; break; }
    std::string nick = c ? c->getNick() : "*";
    std::string user = c ? c->getUser() : "~default";
    return ":" + nick + "!" + user + "@localhost ";
}

void Server::sendNumeric(int fd, int code, const std::string& params, const std::string& trailing) {
    Client* c = getClientByFd(fd);
    std::ostringstream oss;
    oss << ":localhost " << code << " " << (c ? c->getNick() : "*");
    if (!params.empty()) oss << " " << params;
    if (!trailing.empty()) oss << " :" << trailing;
    oss << "\r\n";
    std::string s = oss.str();
    send(fd, s.c_str(), s.size(), 0);
}
