#include "../include/Socket.hpp"
# include <fcntl.h>

Socket::Socket(int port) 
{
	_max_clients = 30;
	_port = port;
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1)
		throw ConnectionException();
}

void 		Socket::initSockets()
{
	int opt = 1;
	//initialise all _client_socket[] to 0 so not checked
	for (int i = 0; i < getMaxClients(); i++)
		_clientSocket[i] = 0;
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
		throw MultipleConnectionsException();
}

void	Socket::bindSocket() {
	_hint.sin_family = AF_INET;
	_hint.sin_port = htons(_port);							// Little endian for intel processors (Host To Network Short)
	// INADDR_ANY contains the IP address of the host. For 
    // server code, this will always be the IP address of 
    // the machine on which the server is running.
	_hint.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(_socketFd, (sockaddr *)&_hint, sizeof(_hint)) == -1)
		throw BindException();
	if (listen(_socketFd, SOMAXCONN) == -1)					// SOMAXCON = max number of communications » 128
		throw ListenException();
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
