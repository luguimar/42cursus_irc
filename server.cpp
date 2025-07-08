#include "irc.hpp"

Server::Server()
{
	_server_port = -1;
	_server_socket_fd = -1;
	_fds.clear();
	_clients.clear();
}

Server::~Server()
{

}

bool Server::_server_live = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "Closing server." << std::endl;
	_server_live = true;
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
	//o segundo campo e o tipo de socket (tem influencia no tipo de "comunicasao") e por fim o protocol que sera sempre 0

	if (_server_socket_fd == -1)
		throw(std::runtime_error("Problem when trying to create the socket."));

	//todas as funcoes poddem dar erro que da return de -1 em caso de erro

	if (setsockopt(_server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		throw(std::runtime_error("Problem when trying to set the socket."));
	//setsockopt: funcao usada com o objetivo de alterar "regras/opcoes" da socket
	//1* campo: fd neste caso da socket
	//2* campo: SOL_SOCKET indica a funcao que o que vamos fazer tem que ser aplicada a socket em si e que e uma "socket level" opcao
	//3* campo: SO_REUSEADDR e a opcao que vamos dar set, que basicamente fas com que certas funcoes , especificamente , o bind() demorem menos tempp
	//4* campo: pelo que percebi este campo e para dar enable or disabel (a nonzero value para enabel & 0 para disabel) mas tem que ser pointer
	//5* campo: e aqui o tamanho do campo 4*

	if (fcntl(_server_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("Problem when trying to set status flags of the socket as \"O_NONBLOCK\"."));
	//fcntl: executa operacoes da propria funcao no fd dado (o fd da socket nest caso)
	//1* campo: fd da socket
	//2* campo: F_SETFL indica que queremos mudar o estado da "file status flags"
	//3* campo: O_NONBLOCK com esta opcao as "file status flags" iram ficar no non-blocking mode que faz com que certas funcoes como read() ou write() funcionem melhor e mais rapido

	if (bind(_server_socket_fd, (struct sockaddr *)&server_adr, sizeof(server_adr)) == -1)
		throw(std::runtime_error("Problem when trying to bind the socket."));
	//bind: serve para atribuir um address e port a uma socket neste caso vamos utilizar o que guardamos em cima
	//1* campo: fd da socket
	//2* campo: a estrutura onde guardamos a informacao do server
	//3* campo: tamanho da estrutura do campo 2*

	//quick aula!!!! a socket pode ser tanto passiva como ativa, se for passiva significa que recebe informacao se for ativa significa que manda informacao (epxlicando de uma forma muito splistica)
	//este tipo de "conexao" da-se o nome "TCP/IP connection" (pesquisar um pouco mais profundo nao vou estar a escrever aqui um testamento)
	//como um pouco intuitivo o server e que vai ser a passive socket e o client e que vai ser a active socket, para fazer o server "passivo" usamos a seguinte funcao listen()

	if (listen(_server_socket_fd, SOMAXCONN) == -1)
		throw(std::runtime_error("Problem when trying to make socket become a passive socket."));
	//listen: como explicado assima listen e usado uma funcao que ja esta "bindada" (bind()) para fazer a socket ficar passive
	//1* campo: fd da socket
	//2* campo: numero de conexoes a aceitar como queremos um numero "infinito" ja que nao ha limite metemos o max (maior possivel) SOMAXCONN

	new_poll.fd = _server_socket_fd;	//fd da socket
	new_poll.events = POLLIN;			//event a fazer neste caso vais ser o POLLIN que e para ler data/informacao
	new_poll.revents = 0;				//historico dos eventos (pelo que percebi) 0 por que ainda nao fizemos nenhum event
	_fds.push_back(new_poll);			//guardar novo poll para ser vigiado pela funcao poll()
}

void Server::startServer(char *port)
{
	(void)port;
	_server_port = std::atoi(port);//guardar a port
	startSocket();

	std::cout << "Server port: " << _server_port << "\nServer socket(fd): " << _server_socket_fd << std::endl;

	while (Server::_server_live == false)
	{
		//std::cout << "a" << std::endl;
		if (poll(&_fds[0], _fds.size(), -1) == -1 && Server::_server_live == false)
			throw(std::runtime_error("The machine heart stopped."));
		//poll: funcao para vigiar fds providos
		//1* campo: list propria (pollfds) com os fds guardados
		//2* campo: tamanha da mesma
		//3* campo: temo max e vigia ate aconter um evento -1 signifa para esperar "infinitamente" ou ate dizer o contrario

		for (size_t i = 0; i < _fds.size(); i++) //verificar todos os fds
		{
			if (_fds[i].revents & POLLIN) //se fd tiver data para ler
			{
				if (_fds[i].fd == _server_socket_fd) // se for o fd da socket significa que e um novo client, se nao e data mandada pelo client
					newClient();
				else
					receivedData(_fds[i].fd);
			}
		}
	}
	closeFd();
}

void Server::newClient()
{
	Client client;
	struct sockaddr_in client_adr;
	struct pollfd new_poll;

	socklen_t len = sizeof(client_adr);

	int client_fd = accept(_server_socket_fd, (sockaddr *)&client_adr, &len);
	//a funcao accept tem como objetivo de estabelecer uma conexao com a socket expecificada no 1* campo devolvendo um fd de outra socket que seria a socket o client
	//1* campo: fd da socket (tem que ser passiva esta explicado assima como se faz uma socket passiva) neste caso e a socket do server
	//2* campo: struct onde a funcao garda informacao importante da socket devolvida
	//3* campo: tamanho dessa mesma struct

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
	//1* campo: fd de onde vem a data
	//2* campo: buffer para guardar a data
	//3* campo: tamanha do bytes a podermos usar dentro do buffer
	//4* campo: flags 0 pq nao usamos nenhuma

	if (data_bytes <= 0) //ver se algo aconteceu com o client e se sim apagar o mesmo
	{
		std::cout << "Something happened to client." << std::endl;
		clearClient(fd);
		close(fd);
	}
	else //data foi recebida com sucesso
	{
		buf[data_bytes] = '\0';
		std::cout << "Received Data: " << buf << "."<< std::endl;
	}
}

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

