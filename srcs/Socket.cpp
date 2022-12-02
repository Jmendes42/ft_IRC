#include "../include/Socket.hpp"

Socket::Socket(int port) {

	_max_clients = 30;
	_port = port;
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1) {
		MSG("Cant't create socket");
		exit(1);
	}
}

void 		Socket::init_sockets()
{
	int opt = 1;
	//initialise all _client_socket[] to 0 so not checked
	for (int i = 0; i < getMaxClients(); i++)
		_clientSocket[i] = 0;
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		MSG("setsockopt");
		exit(2);
	}
}

void	Socket::_bind() {
	_hint.sin_family = AF_INET;
	_hint.sin_port = htons(_port);									// Little endian for intel processors (Host To Network Short)
	// INADDR_ANY contains the IP address of the host. For 
    // server code, this will always be the IP address of 
    // the machine on which the server is running.
	_hint.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(_socketFd, (sockaddr *)&_hint, sizeof(_hint)) == -1) {
		MSG("Can't bind int to IP/port");
		exit(2);
	}
	if (listen(_socketFd, SOMAXCONN) == -1) {						// SOMAXCON = max number of communications » 128
		MSG("Can't listen!");
		exit(3);
	}
	_clientSize = sizeof(_hint);
}

void	Socket::sets()
{
	// clear the socket set
	FD_ZERO(&_readfds);
	// add master socket to set
	FD_SET(_socketFd, &_readfds);
	_max_sd = _socketFd;
	// add child sockets to set
	for (int i = 0; i < getMaxClients(); i++)
	{
		// socket descriptor
		_sd = _clientSocket[i];
			
		// if valid socket descriptor then add to read list
		if(_sd > 0)
			FD_SET(_sd, &_readfds);
			
		// highest file descriptor number, need it for the select function
		if(_sd > _max_sd)
			_max_sd = _sd;
	}
}



// void	Socket::_clientSet() {

// 	// The listen system call allows the process to listen 
//     // on the socket for connections. 
//     // The program will be stay idle here if there are no 
//     // incomming connections.
// 	if (listen(_socketFd, SOMAXCONN) == -1) {						// SOMAXCON = max number of communications » 128
// 		MSG("Can't listen!");
// 		exit(3);
// 	}
// 	_clientSize = sizeof(_client);
// 	_clientSocket = accept(_socketFd, (sockaddr *)&_client, &_clientSize);
// 	if (_clientSocket == -1) {
// 		MSG("Problem with client connecting!");
// 		exit(4);
// 	}
// 	close(_socketFd);
// 	ft_memset(_host, 0, NI_MAXHOST);
// 	ft_memset(_svc, 0, NI_MAXSERV);
// }