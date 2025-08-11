#include "Server.hpp"

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

//getters
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

//sighandler so no crash
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "Closing server." << std::endl;
	_server_live = true;
}

//cmds
void Server::join(int fd_c, std::vector<std::string> cmd)
{
	std::cout << "Entered Join" << std::endl;
    if (cmd.size() < 2)                               // 461 ERR_NEEDMOREPARAMS
        return (void)send(fd_c, "461 JOIN :Need more params\r\n", 31, 0);

    /* separar vários canais */
    std::istringstream ss(cmd[1]);
    std::string chanName;
    while (std::getline(ss, chanName, ','))
    {
        if (chanName.empty() || (chanName[0] != '#' && chanName[0] != '&'))
        {
            std::string err = "476 " + chanName + " :Bad channel name\r\n";
            send(fd_c, err.c_str(), err.length(), 0);
            continue;
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

        /* Mensagens IRC mínimas: JOIN e NAMES */
        std::string joinMsg = ":localhost JOIN " + chanName + "\r\n";
        chan->broadcast(joinMsg, -1);                 // avisa toda a gente

        /* 353 RPL_NAMREPLY */
        std::string names = "= " + chanName + " :";
        for (std::set<int>::iterator it = chan->getMembers().begin();
             it != chan->getMembers().end(); ++it)
        {
            Client *cl = getClientByFd(*it);
            names += cl ? cl->getIp() + " " : "";     // ou usa nick se já tiveres
        }
        names += "\r\n";
        send(fd_c, ("353 " + names).c_str(), names.length() + 4, 0);
        /* 366 RPL_ENDOFNAMES */
        send(fd_c, ("366 " + chanName + " :End of /NAMES list\r\n").c_str(),
                   chanName.length() + 30, 0);
    }
}

void Server::privmsg(int fd_c, std::vector <std::string> cmd)
{
	std::cout << "Entered Privmsg" << std::endl;
	if (cmd.size() < 3)
	{
		if(cmd.size() <= 1)
			send(fd_c, "ERR_NORECIPIENT (411)\r\n", 25, 0);
		else
			send(fd_c, "ERR_NOTEXTTOSEND (412)\r\n", 26, 0);
		return ;
	}
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
	//else // e um client
}

int	check_nick(const char *nick)
{
	int	i;

	i = -1;
	while (nick[++i] != '\0')
	{
		if (nick[0] == '#' || nick[0] == '&' || nick[0] == ':' || (nick[0] >= '0' && nick[0] <= '9'))
			return (1);
		else if (nick[i] == ' ')
			return (1);
	}
	return (0);
}

void Server::setnick(int fd_c, std::vector<std::string> cmd)
{
	std::cout << "Setting up nickname" << std::endl;
	if (cmd.size() < 2)
		return (void)send(fd_c, "ERR_NONICKNAMEGIVEN (431)\r\n", 29, 0); 
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNick() == cmd[1])
			return (void)send(fd_c, "ERR_NICKNAMEINUSE (433)\r\n", 27, 0); 
	}
	if (check_nick(cmd[1].c_str()))
		return (void)send(fd_c, "ERR_ERRONEUSNICKNAME (432)\r\n", 30, 0); 
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd_c)
		{
			if (_clients[i].getNick() == "")
			{
				std::string msg = "Nickname successfully registered as " + cmd[1] + "\r\n";
				send(fd_c, msg.c_str(), (40 + cmd[1].size()), 0);
                _clients[i].setNick(cmd[1]);
			}
			else
			{
             	/*std::string msg = ":" + _clients[i].getNick() + "!" + _clients[i].getUser(); + "@localhost " + cmd[0] + " " + cmd[1] + "\r\n";
                std::cout << msg << std::endl;*/
				std::string msg = _clients[i].getNick() + "changed his nickname to " + cmd[1] + "\r\n";
				send(fd_c, msg.c_str(), (_clients[i].getNick().size() + 28 + cmd[i].size()), 0);
			}
			_clients[i].setOldNick();
			_clients[i].setNick(cmd[1]);
            //:Alejandro!Ale_j@localhost NICK hello2
            //:oldname!user@localhost NICK newnick
		}
	}
	// falta mandar a mensagem de sucesso pro server
}
//server related
void Server::startServer(char *port)
{
	for (size_t i = 0; port[i]; i++)
		if (isalnum(port[i]) == 0)
			throw(std::runtime_error("Please try to use only numers for the port."));

	_server_port = std::atoi(port);//guardar a port
	static int flag_newc;

	startSocket();

	std::cout << "Server port: " << _server_port << "\nServer socket(fd): " << _server_socket_fd << std::endl;

	while (Server::_server_live == false)
	{
		//std::cout << "a" << std::endl;
		if (poll(&_fds[0], _fds.size(), -1) == -1 && Server::_server_live == false)
			throw(std::runtime_error("The machine heart stopped."));
		//poll: funcao para vigiar fds providos

		for (size_t i = 0; i < _fds.size(); i++) //verificar todos os fds
		{
			if (_fds[i].revents & POLLIN) //se fd tiver data para ler
			{
				if (_fds[i].fd == _server_socket_fd) // se for o fd da socket significa que e um novo client, se nao e data mandada pelo client
				{
					newClient();
					flag_newc = 1;
					std::cout << "Flag seted to: "<< flag_newc << std::endl;
				}
				else
				{
					receivedData(_fds[i].fd);
					flag_newc = 0;
					std::cout << "Flag seted to: "<< flag_newc << std::endl;
				}
				std::cout << "Flag Status: "<< flag_newc << std::endl;
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
	std::cout << "Ip guardado em string: "<< inet_ntoa(client_adr.sin_addr) << std::endl;
	client.setIp(inet_ntoa((client_adr.sin_addr)));	//guardar o ip na estrutura do client (a funcao inet_ntoa() pega num adress/ip e transforma num string e devolve)
	_clients.push_back(client);							//guardar novo client no vector dos clients
	_fds.push_back(new_poll);							//guardar novo poll para vigiar

	std::cout << "New Client." << std::endl;
}

void Server::receivedData(int fd)
{
	char buf[1024];							// buffer para a data que vamos receber
	memset(buf, 0, sizeof(buf));	//limpar o buffer

	ssize_t data_bytes = recv(fd, buf, sizeof(buf) - 1, 0);
	//a funcao recv tem como objetivo de receber a data do fd dado

	if (data_bytes <= 0) //ver se algo aconteceu com o client e se sim apagar o mesmo
	{
		std::cout << "Something happened to client." << std::endl;
		clearClient(fd);
		close(fd);
	}
	else //data foi recebida com sucesso
	{
		buf[data_bytes] = '\0';
		std::cout << "Received Data: " << buf << std::endl;
		//parser para a data e exe o cmd respetivo se for cmd
		parseExec(fd, static_cast<std::string>(buf));
	}
}

void Server::parseExec(int fd_c, std::string buf)
{
	for (size_t i = 0; i != buf.length(); i++)
		if (buf[i] == '\n')
			buf[i] = ' ';

	std::string	token_value;
	std::istringstream buff(buf);
	std::vector<std::string> tokens;

	while(std::getline(buff, token_value, ' '))
		tokens.push_back(token_value);

	for (int i = 0; i!= static_cast<int>(tokens[0].size()); i++)
			tokens[0][i] = std::toupper(tokens[0][i]);

	//prob vai-se fazer um index das funcoes todas para esta parte


	if (tokens[0] == "JOIN")
		join(fd_c, tokens);
	else if (tokens[0] == "PRIVMSG")
		privmsg(fd_c, tokens);
	else if (tokens[0] == "NICK")
		setnick(fd_c, tokens);
	/*else if (tokens[0] = "USER")
		usercmd();
	else if (tokens[0] = "PASS")
		passcmd() */
}

//"frees"
void Server::closeFd()
{
	for (size_t i = 0; i < _clients.size() ; i++)  //fechar todos os fds
		close(_clients[i].getFd());
	if (_server_socket_fd != -1)				//fechar o fd do server
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
