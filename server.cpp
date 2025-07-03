#include "irc.hpp"

Server::Server()
{
	_server_port = -1;
	_server_socket_fd = -1;
}

Server::~Server()
{

}

void Server::startSocket()
{
	struct sockaddr_in server_adr;		//struct propria para guardar informacao importate sobre o address do server (normalmente usada para IPv4 (address))
	struct pollfd new_poll;				//struct propria para ser usada com a funcao poll() que ira ajudar a trabalhar com varios fds ao mesmo tempo e tambem tem como uso vigiar os fds guardados

	server_adr.sin_family = AF_INET; 					 //int que representa o tipo de address do server 'AF_INET' indica que e IPv4
	server_adr.sin_port = htons(_server_port); //16-bit int que guarda a port do server em "network byte order" por isso e que se usa o htons para transformar a mesma
	server_adr.sin_addr.s_addr = INADDR_ANY; 			 //por fim o proprio address do server como nos queremos que se conecte a 'todas' usamos a flag 'INADDR_ANY'

	_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	//socket devolve um fd para interargir com a mesma, o primeiro campo e o tipo de address
	//o segundo campo e o tipo de socket (tem influencia no tipo de "comunicasao") e por fim o protocol que sera sempre 0

	if (_server_socket_fd < 0)
		return ; //error

	int enable = 1;

	//todas as funcoes poddem dar erro que da return de -1 em caso de erro

	if (setsockopt(_server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
		return ; //error
	//setsockopt: funcao usada com o objetivo de alterar "regras/opcoes" da socket
	//1* campo: fd neste caso da socket
	//2* campo: SOL_SOCKET indica a funcao que o que vamos fazer tem que ser aplicada a socket em si e que e uma "socket level" opcao
	//3* campo: SO_REUSEADDR e a opcao que vamos dar set, que basicamente fas com que certas funcoes , especificamente , o bind() demorem menos tempp
	//4* campo: pelo que percebi este campo e para dar enable or disabel (a nonzero value para enabel & 0 para disabel) mas tem que ser pointer
	//5* campo: e aqui o tamanho do campo 4*

	if (fcntl(_server_socket_fd, F_SETFL, O_NONBLOCK) < 0)
		return ;
	//fcntl: executa operacoes da propria funcao no fd dado (o fd da socket nest caso)
	//1* campo: fd da socket
	//2* campo: F_SETFL indica que queremos mudar o estado da "file status flags"
	//3* campo: O_NONBLOCK com esta opcao as "file status flags" iram ficar no non-blocking mode que faz com que certas funcoes como read() ou write() funcionem melhor e mais rapido

	if (bind(_server_socket_fd, (struct sockaddr *)&server_adr, sizeof(server_adr) < 0))
		return ;
	//bind: serve para atribuir um address e port a uma socket neste caso vamos utilizar o que guardamos em cima
	//1* campo: fd da socket
	//2* campo: a estrutura onde guardamos a informacao do server
	//3* campo: tamanho da estrutura do campo 2*

	//quick aula!!!! a socket pode ser tanto passiva como ativa, se for passiva significa que recebe informacao se for ativa significa que manda informacao (epxlicando de uma forma muito splistica)
	//este tipo de "conexao" da-se o nome "TCP/IP connection" (pesquisar um pouco mais profundo nao vou estar a escrever aqui um testamento)
	//como um pouco intuitivo o server e que vai ser a passive socket e o client e que vai ser a active socket, para fazer o server "passivo" usamos a seguinte funcao listen()

	if (listen(_server_socket_fd, SOMAXCONN) < 0)
		return ;
	//listen: como explicado assima listen e usado uma funcao que ja esta "bindada" (bind()) para fazer a socket ficar passive
	//1* campo: fd da socket
	//2* campo: numero de conexoes a aceitar como queremos um numero "infinito" ja que nao ha limite metemos o max (maior possivel) SOMAXCONN

	new_poll.fd = _server_socket_fd;	//fd da socket
	new_poll.events = POLLIN;			//event a fazer neste caso vais ser o POLLIN que e para ler data/informacao
	new_poll.revents = 0;				//historico dos eventos (pelo que percebi) 0 por que ainda nao fizemos nenhum event
	fds.push_back(new_poll);			//guardar no novo poll para usar mais tarde
}

void Server::startServer(char *port)
{
	_server_port = std::atoi(port);
	startSocket();

	std::cout << "Server port: " << _server_port << "\nServer socket(fd): " << _server_socket_fd << std::endl;
}