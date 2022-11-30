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
 #include <sys/poll.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>

#include <sstream>
#include<string>

#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE 1
#define FALSE 0
#define PORT 8888

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

	//added for select()
	int client_socket[30];
	int max_clients, max_sd, sd, new_socket, valread;
	char buffer[1025];
	    //set of socket descriptors
	fd_set readfds;


	public:
		Socket();
		Socket(int port);				// Optionally it can also receive an IP if necessary

		void 		init_sockets();
		//void		_chat();
		void		_bind();
		void		_clientSet();


		void		sets();
		void		activity();
		
		std::string	&getMessage()		{ return _message; };
		int			&getClientSocket()	{ return _clientSocket; };
		int			&getMaxClients()	{ return max_clients; };
		int			&getSocketFd()	{ return _socketFd; };


};

template<typename T>
void	MSG(T msg) { std::cout << "Message: " << msg << std::endl; };
void	*ft_memset(void *, int, size_t);

#endif

