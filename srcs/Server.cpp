#include "../include/Server.hpp"

void	Server::activity()
{
	char		buffer[1024];
	std::string message = "001 isousa \r\n";

	_sock.getActivity() = select(_sock.getMaxSd() + 1 , &_sock.getReadFds() , NULL , NULL , NULL);
	if ((_sock.getActivity() < 0) && (errno != EINTR))
	{
		MSG("Select Error");
		exit(2);
	}
	// If something happened on the master socket,
	// then its an incoming connection
	if (FD_ISSET(_sock.getSocketFd(), &_sock.getReadFds()))
	{
		_sock.setNewSocket(accept(_sock.getSocketFd(), (struct sockaddr *)&_sock.getHint(), (socklen_t*)&_sock.getClientSize()));
		if (_sock.getNewSocket() < 0)
		{
			MSG("Accepting Error");
			exit(2);
		}
		// Inform user of socket number - used in send and receive commands
		std::cout << "New connection , socket fd is " << _sock.getNewSocket() 
			<< ", IP is: " << inet_ntoa(_sock.getHint().sin_addr) 
				<< ", port: " << ntohs(_sock.getHint().sin_port) << std::endl;
		// Send new connection greeting message
		if (send(_sock.getNewSocket(), message.c_str(), strlen(message.c_str()), 0) != strlen(message.c_str()))
		{
			MSG("Sending Message Error");
			exit(2);
		}
		// Add new socket to array of sockets
		for (int i = 0; i < _sock.getMaxClients(); i++)
		{
			// If position is empty
			if (_sock.getClientSocket(i) == 0 )
			{
				_sock.setClientSocket(i, _sock.getNewSocket());
				printf("Adding to list of sockets as %d\n" , i);
				break;
			}
		}
	}
	// Else its some IO operation on some other socket
	MSG(_sock.getMaxClients());
	for (int i = 0; i < _sock.getMaxClients(); i++)
	{
		_sock.setSd(_sock.getClientSocket(i));
		if (FD_ISSET(_sock.getSd() , &_sock.getReadFds()))
		{
			// Check if it was for closing , and also read the
			// incoming message
			_sock.setValRead(read(_sock.getSd(), buffer, 1024));
			if (_sock.getValRead() == 0)
			{
				// Somebody disconnected , get his details and print
				getpeername(_sock.getSd(), (struct sockaddr*)&_sock.getHint(), (socklen_t*)&_sock.getClientSize());
				printf("Host disconnected , ip %s , port %d \n", inet_ntoa(_sock.getHint().sin_addr), ntohs(_sock.getHint().sin_port));
					
				// Close the socket and mark as 0 in list for reuse
				close(_sock.getSd());
				_sock.setClientSocket(i, 0);
			}
			// Echo back the message that came in
			else
			{
				_interpreter(std::string(buffer, 0, _sock.getValRead()), _sock.getSd());
				/*MSG(_sock.getSd());
				send(_sock.getSd(), ":jmendes!jmendes@localhost 353 jmendes = #tardiz :@jmendes\n"
				":jmendes!jmendes@localhost 366 jmendes #tardiz :End of /NAMES list\n"
				":jmendes!jmendes@localhost JOIN :#tardiz\n", 168, 0);*/
			}
		}
	}
}

/*void	Server::_chat() {

    int clientSocket = _sock.getClientSocket();
    std::string msg = _sock.getMessage();
	char buf[4096];

	while (true) {

		MSG("Loop");
		// Clear the buffer
		ft_memset(buf, 0, 4096);
		// Wait for a message
		int bytesRecv = recv(clientSocket, buf, 4096, 0);
		if (bytesRecv == -1) {
			MSG("There was a connection issue!");
			break;
		}
		if (bytesRecv == 0) {
			MSG("The client has disconnected");
			break;
		}
		// Client message
		msg = std::string(buf, 0, bytesRecv);
        _interpreter(msg, clientSocket);
		std::cout << "Received: " << msg << std::endl;		// Display msg
	}
	close(clientSocket);
}*/

void    Server::_sockSet() {

	_sock.init_sockets();
    _sock._bind();

	// _sock._clientSet();
}

void    Server::_interpreter(std::string const &msg, int const &clientSocket) {
    std::string cmd = msg.substr(0, msg.find(' '));

	if (!cmd.compare("PASS") || !cmd.compare("NICK") || !cmd.compare("USER")) {

		MSG(msg);
		_clientHandler.addClient(msg, _password);
		if (!cmd.compare("NICK")) {
			std::string nick = "001 " + msg.substr(4, msg.find('\n'));
			send(clientSocket, nick.c_str(), nick.length(), 0);
		}
	}
	if (!cmd.compare("JOIN")) {
		send(clientSocket, ":jmendes!jmendes@localhost 353 jmendes = #tardiz :@jmendes\n"
			":jmendes!jmendes@localhost 366 jmendes #tardiz :End of /NAMES list\n"
			":jmendes!jmendes@localhost JOIN :#tardiz\n", 168, 0);
		_channelHandler.addChannel(msg, new Client());
	}
	if (!cmd.compare("PRIVMSG")) {
		_clientHandler.privateMsg(msg);
	}
}
