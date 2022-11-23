#include "../include/Socket.hpp"

Socket::Socket(int port) {

	_port = port;
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1) {
		MSG("Cant't create socket");
		exit(1);
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
}

void	Socket::_clientSet() {

	// The listen system call allows the process to listen 
    // on the socket for connections. 
    // The program will be stay idle here if there are no 
    // incomming connections.
	if (listen(_socketFd, SOMAXCONN) == -1) {						// SOMAXCON = max number of communications » 128
		MSG("Can't listen!");
		exit(3);
	}
	_clientSize = sizeof(_client);
	_clientSocket = accept(_socketFd, (sockaddr *)&_client, &_clientSize);
	if (_clientSocket == -1) {
		MSG("Problem with client connecting!");
		exit(4);
	}
	close(_socketFd);
	ft_memset(_host, 0, NI_MAXHOST);
	ft_memset(_svc, 0, NI_MAXSERV);
}
