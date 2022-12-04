#include "../include/Server.hpp"

void	Server::activity()
{
	char		buffer[1024];
	std::string message;

	_sock.setActivity(select(_sock.getMaxSd() + 1 , &_sock.getReadFds() , NULL , NULL , NULL));
	if ((_sock.getActivity() < 0) && (errno != EINTR))
	{
		MSG("Select Error");
		throw ConnectionException();
	}
	// If something happened on the master socket,
	// then its an incoming connection
	if (FD_ISSET(_sock.getSocketFd(), &_sock.getReadFds()))
	{
		_sock.setNewSocket(accept(_sock.getSocketFd(), (struct sockaddr *)&_sock.getHint(), (socklen_t*)&_sock.getClientSize()));
		if (_sock.getNewSocket() < 0)
		{
			MSG("Accepting Error");
			throw ConnectionException();
		}
		// Inform user of socket number - used in send and receive commands
		std::cout << "New connection , socket fd is " << _sock.getNewSocket() 
			<< ", IP is: " << inet_ntoa(_sock.getHint().sin_addr) 
				<< ", port: " << ntohs(_sock.getHint().sin_port) << std::endl;
		// Send new connection greeting message
		/*if (send(_sock.getNewSocket(), message.c_str(), strlen(message.c_str()), 0) != strlen(message.c_str()))
		{
			MSG("Sending Message Error");
			throw ConnectionException();
		}*/
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
			else {
				try
				{
					MSG(buffer);
					_interpreter(std::string(buffer, 0, _sock.getValRead()), _sock.getSd());
				}
				catch(std::exception &error)
				{
					MSG(error.what());
				}
			}
		}
	}
}

void    Server::_sockSet() {
	_sock.initSockets();
    _sock.bindSocket();
}

void    Server::_interpreter(std::string const &msg, int const &sockFd) {
    std::string cmd = msg.substr(0, msg.find(' '));

	MSG(msg);
	if (!cmd.compare("PASS"))
		_clientHandler.addClient(msg, _password, sockFd);
	else if (!cmd.compare("NICK")) {
		std::string	nick = msg.substr(5, msg.find('\0', 5));
		MSG("LEN");
		MSG(nick.length());
		MSG("Original nick " + nick + "PP");

		nick.erase(nick.find('\r'), 2);
		_clientHandler.finder(sockFd, "")->setNick(nick);
		nick = "001 " + nick + "\n";
		send(sockFd, nick.c_str(), nick.length(), 0);
	}
	else if (!cmd.compare("USER")) {
		std::string	user = msg.substr(5, msg.find('\n'));
		_clientHandler.finder(sockFd, "")->setUser(user);
	}
	else if (!cmd.compare("JOIN")) {
		send(sockFd, ":jmendes!jmendes@localhost 353 jmendes = #tardiz :@jmendes\n"
			":jmendes!jmendes@localhost 366 jmendes #tardiz :End of /NAMES list\n"
			":jmendes!jmendes@localhost JOIN :#tardiz\n", 168, 0);
		_channelHandler.addChannel(msg, _clientHandler.finder(sockFd, ""));
	}
	else if (!cmd.compare("PRIVMSG")) {
		// Here private msg
	std::string sendMsg;
	sendMsg = ":ines!ines@localhost 413 " + msg;



		MSG("+" + msg.substr(8, msg.find(' ', 8) - 8) + "+");
		int fd = _clientHandler.finder(0, msg.substr(8, msg.find(' ', 8) - 8))->getFd();
		MSG(sendMsg);
		send(fd,":ines!ines@localhost 413 ines Ola\n" , 35, 0);
		_clientHandler.privateMsg(msg);
	}
}
