#include "Socket.hpp"

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
	inet_pton(AF_INET, "0.0.0.0", &_hint.sin_addr);					// Converts a string number to an array of ints. 0.0.0.0 stands for "any address"
	if (bind(_socketFd, (sockaddr *)&_hint, sizeof(_hint)) == -1) {
		MSG("Can't bind int to IP/port");
		exit(2);
	}
}

void	Socket::_clientSet() {

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
	memset(_host, 0, NI_MAXHOST);
	memset(_svc, 0, NI_MAXSERV);
	_result = getnameinfo((sockaddr *)&_client, sizeof(_client), _host, NI_MAXHOST, _svc, NI_MAXSERV, 0);
	if (_result) 
		std::cout <<_host << " connected on " << _svc << std::endl;
	else {
		inet_ntop(AF_INET, &_client.sin_addr, _host, NI_MAXHOST);
		std::cout << _host << " connected on " << ntohs(_client.sin_port) << std::endl;
	}
}
