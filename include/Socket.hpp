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
# include <sys/poll.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <errno.h>
# include <sys/time.h> // FD_SET, FD_ISSET, FD_ZERO macros

class Socket {


	sockaddr_in 	_hint;
	int				_port;
	int				_socketFd;

	sockaddr_in		_client;
	socklen_t		_clientSize;
	//int				_clientSocket;
	char			_svc[NI_MAXSERV];
	char			_host[NI_MAXHOST];

	int				_result;
	std::string		_message;

	//added for select()
	int				_activity;
	int				_clientSocket[30];
	int				_max_clients, _max_sd, _sd, _valRead;
	int				_newSocket;
	    //set of socket descriptors
	fd_set _readfds;

	public:
		Socket();
		Socket(int port);				// Optionally it can also receive an IP if necessary

		void 		init_sockets();
		//void		_chat();
		void		_bind();
		void		_clientSet();


		void		sets();
		

		socklen_t	&getClientSize()				{return _clientSize;};
		sockaddr_in	&getHint()						{return _hint;};
		int			&getSd()						{return _sd;};
		int			&getMaxSd()						{return _max_sd;};
		std::string	&getMessage()					{return _message;};
		int			&getValRead()					{return _valRead;};
		fd_set		&getReadFds()					{return _readfds;};
		int			&getSocketFd()					{return _socketFd;};
		int			&getActivity()					{return _activity;};
		int			&getNewSocket()					{return _newSocket;};
		int			&getMaxClients()				{return _max_clients;};
		int			&getClientSocket(const int &i)	{return _clientSocket[i];};

		void	setSd(const int &sd)							{_sd = sd;};
		void	setValRead(const int &val)						{_valRead = val;};
		void	setNewSocket(const int &ns)						{_newSocket = ns;};
		void	setClientSocket(const int &i, const int &sock)	{_clientSocket[i] = sock;};

};

template<typename T>
void	MSG(T msg) { std::cout << "Message: " << msg << std::endl; };
void	*ft_memset(void *, int, size_t);

#endif
