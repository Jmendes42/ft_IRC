#include <string>
#include <iostream>

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

	// The “sockaddr_in” structure allows binding a socket with the desired address 
	// so that a server can listen to the clients’ connection requests.

	sockaddr_in 	_hint;
	int				_port;
	int				_socketFd;

	sockaddr_in		_client;
	socklen_t		_clientSize;
	int				_clientSocket;
	char			_svc[NI_MAXSERV];
	char			_host[NI_MAXHOST];

	int				_result;
	std::string		_message;

	public:

		Socket(int port);				// Optionally it can also receive an IP if necessary

		void	_chat();
		void	_bind();
		void	_clientSet();
		/*inputStream getInputStream();	// Get input from user
										// This input stream method will return the 
										// InputStream representing the data attached to 
										// this socket. It also throws an exception.

		outputStream getOutputStream();	// Get output from user
										// This output stream method will return the 
										// OutputStream representing the data attached to 
										// this socket. It also throws an exception.
		// ^ Make sure the object must be returned every time you call the 
		// above methods to avoid errors.*/
};


void	Socket::_bind() {

	// Bind the socket to ip and port
	_hint.sin_family = AF_INET;

	// htons() makes sure that the numbers are stored in memmory
	// in the network byte order, which is
	// the most significant byte first (big endian)
	_hint.sin_port = htons(_port);									// Little endian for intel processors (Host To Network Short)
	inet_pton(AF_INET, "0.0.0.0", &_hint.sin_addr);					// Converts a string number to an array of ints. 0.0.0.0 stands for "any address"
	if (bind(_socketFd, (sockaddr *)&_hint, sizeof(_hint)) == -1) {
		MSG("Can't bind int to IP/port");
		exit(2);
	}
}

void	Socket::_clientSet() {

	// https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-listen-prepare-server-incoming-client-requests
	// listen() indicates a readiness to accept client connection requests,
	// creates a connection request queue to incoming connection requests.
	// Once full, additional connection requests are rejected. 
	if (listen(_socketFd, SOMAXCONN) == -1) {						// SOMAXCON = max number of communications » 128

		MSG("Can't listen!");
		exit(3);
	}
	_clientSize = sizeof(_client);

	// https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-accept-accept-new-connection-socket
	// accept()is used by a server to accept a connection request from a client.
	// When a connection is available, the socket created is ready for use to
	// read data from the process that requested the connection.
	// The call accepts the first connection on its queue of pending connections for the given socket.
	// It creates a new socket descriptor with the same properties as socket and returns it.
	// If the queue has no pending connection requests it blocks the caller unless it's in nonblocking mode.
	_clientSocket = accept(_socketFd, (sockaddr *)&_client, &_clientSize);

	if (_clientSocket == -1) {
		MSG("Problem with client connecting!");
		exit(4);
	}
	close(_socketFd);
	memset(_host, 0, NI_MAXHOST);
	memset(_svc, 0, NI_MAXSERV);

	// https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getnameinfo-get-name-information
	// getnameinfo() translates a socket address to a node name and service location,
	// it looks up an IP address and port number provided by the caller in the DNS and system-specific database,
	// and returns text strings for both in buffers provided by the caller.
	// Upon successful completion it returns the node and service names, if requested, in the buffers provided.
	// The returned names are always null-terminated strings.
	// A zero return value for indicates successful completion; a non-zero return value indicates failure.
	_result = getnameinfo((sockaddr *)&_client, sizeof(_client), _host, NI_MAXHOST, _svc, NI_MAXSERV, 0);
	if (_result) 
		std::cout <<_host << " connected on " << _svc << std::endl;
	else {
		// https://www.ibm.com/docs/en/zos/2.3.0?topic=lf-inet-ntop-convert-internet-address-format-from-binary-text
		// The inet_ntop() function converts from an Internet address in binary format,
		// specified by src, to standard text format, and places the result in dst,
		inet_ntop(AF_INET, &_client.sin_addr, _host, NI_MAXHOST);
		std::cout << _host << " connected on " << ntohs(_client.sin_port) << std::endl;
	}

}

void	Socket::_chat() {

	std::string nick;
	std::string user;
	char buf[4096];
	int	counter = 0;

	while (true) {




		// Clear the buffer
		memset(buf, 0, 4096);
		// Wait for a message
		int bytesRecv = recv(_clientSocket, buf, 4096, 0);
		if (bytesRecv == -1) {
			MSG("There was a connection issue!");
			break;
		}
		if (bytesRecv == 0) {
			MSG("The client has disconnected");
			break;
		}

		if (counter == 2) {
			//send(_clientSocket, "jmendes!jmendes@0\n", 17 + 1, 0);
			counter++;
		}
		else if (!strncmp(buf, "NICK ", 5)) {
				nick = std::string(buf, 0, bytesRecv);
				counter++;
		}
		else if (!strncmp(buf, "USER", 4)) {
				user = std::string(buf, 0, bytesRecv);
				send(_clientSocket, buf, bytesRecv + 1, 0);				// Resend msg
				counter++;
		}
		// Client message
		_message = std::string(buf, 0, bytesRecv);

		std::cout << "Received: " << _message << std::endl;		// Display msg
	//	send(_clientSocket, buf, bytesRecv + 1, 0);				// Resend msg
		if (_message == "/exit") {								// Not worki'n
			MSG("The client has disconnected");
			break;
		}
		send(_clientSocket, "001 jmendes\n", 12 + 1, 0);
		//send(_clientSocket, "jmendes!jmendes@0\n",18 + 1, 0);
	}
	MSG(user);
	close(_clientSocket);
}

Socket::Socket(int port) {

	_port = port;
	// Create a socket 
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1) {
		MSG("Cant't create socket");
		exit(1);
	}
}

int	main() {

	Socket sock(54000);

	sock._bind();
	sock._clientSet();
	sock._chat();
	return 0;
}
