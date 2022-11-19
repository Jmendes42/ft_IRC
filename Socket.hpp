#ifndef SOCKETCLASS_HPP
# define SOCKETCLASS_HPP

# include <vector>
# include <string>
# include <iostream>

# include <netdb.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>

class Socket {

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
		Socket();
		Socket(int port);				// Optionally it can also receive an IP if necessary

		void		_chat();
		void		_bind();
		void		_clientSet();
		
		std::string	&getMessage()		{ return _message; };
		int			&getClientSocket()	{ return _clientSocket; };
};

template<typename T>
void	MSG(T msg) { std::cout << "Message: " << msg << std::endl; };

#endif

