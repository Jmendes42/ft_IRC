#include <string>
#include <iostreams>

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

template<typename T>
void	MSG(T msg) {
	std::cout << msg << std::endl;
}

class Socket {

	sockaddr_in 	_hint;
	int				_port;
	std::string		_message;
	int				_socketFd;

	sockaddr_in		_client;
	socklen_t		_clientSize;
	char			_host[NI_MAXHOST];
	char			_svc[NI_MAXSERV];

	public:
		inputStream getInputStream();	// Get input from user
										// This input stream method will return the 
										// InputStream representing the data attached to 
										// this socket. It also throws an exception.

		outputStream getOutputStream();	// Get output from user
										// This output stream method will return the 
										// OutputStream representing the data attached to 
										// this socket. It also throws an exception.
		// ^ Make sure the object must be returned every time you call the 
		// above methods to avoid errors.
};


Socket::Socket() {

	// Create a socket 
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1) {
		MSG("Cant't create socket");
		exit(1);
	}
	// Bind the socket to ip and port
	_hint.sin_family = AF_INET;
	_hint.sin_port = htons(54000);
	inet_pton(AF_INET, "0.0.0.0", &hit.sin_addr);
	if (bind(_socketFd, AF_INET, &hint, sizeof(hint)) == -1) {

		MSG("Can't bind int to IP/port");
		exit(2);
	}
	// Mark for listening in
	if (listen(_socketFd, SOMAXCONN) == -1) {

		MSG("Can't listen!");
		exit(3);
	}
}
int	main() {

	// Accept call
	// Close listening socket
	// While recien«ving display, echo message
	// close socket
	


	return 0;
}
