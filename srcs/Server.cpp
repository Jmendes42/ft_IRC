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
					interpreter(std::string(buffer, 0, _sock.getValRead()), _sock.getSd());
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

void    Server::interpreter(std::string const &msg, int const &sockFd) {
    std::string cmd = msg.substr(0, msg.find(' '));

	MSG(msg);
	if (!cmd.compare("NICK"))
		setClientNick(msg, sockFd);
	else if (!cmd.compare("USER"))
		setClientUser(msg, sockFd);
	else if (!cmd.compare("JOIN"))
		joinChannel(msg, sockFd);
	else if (!cmd.compare("PRIVMSG"))
		privMsg(msg, sockFd);
	else if (!cmd.compare("PASS"))
		_clientHandler.addClient(msg, _password, sockFd, std::string(inet_ntoa(_sock.getHint().sin_addr)));
}

void    Server::joinChannel(const std::string &msg, const int &sockFd) {
	// JOIN can come with multiple channels. Parse the message in order to not create a channel 
	// wich has a name with all the names compiled -> split JOIN #c,#b,#c,#b,#c,#b,#c,#b

	// If a channel has more than one operator? 

	// See codes 403, 404, 405 --- 651

	//When a user nickname changes, update on the participants list
	std::string channelMsg;			// Confirm message user/nick
	std::string channelName = msg.substr(5, msg.find('\0', 5));
	std::string ip = _clientHandler.finder(sockFd, "")->getIp();
	std::string nick = _clientHandler.finder(sockFd, "")->getNick();
	std::string user = _clientHandler.finder(sockFd, "")->getUser();

	MSG(user);
	channelName.erase(channelName.find('\r'), 2);
	if (channelName[0] != '#')
		channelName = "#" + channelName;
	if (_channelHandler.finder(channelName) != NULL) {
		std::vector<Client *>::iterator	it;
		std::vector<Client *>			users;
		std::string						joinMsg;

		users = _channelHandler.finder(channelName)->getUsers();
		joinMsg = ':' + nick + '!' + user + '@' + ip + " JOIN " + channelName + '\n';
		for (it = users.begin(); it != users.end(); it++) {
			int fd = (*it)->getFd();
			send(fd, joinMsg.c_str(), joinMsg.length(), 0);	// Send message to all users that there is a new member
		}
		_channelHandler.finder(channelName)->addUser(_clientHandler.finder(sockFd, ""));
	}
	else
		_channelHandler.addChannel(channelName, _clientHandler.finder(sockFd, ""));

	channelMsg = ":" + nick + "!" + user + "@" + ip + " JOIN :" + channelName + "\n";
	channelMsg += "353 " + nick + " = " + channelName + " :@";
	channelMsg += _channelHandler.finder(channelName)->getUsersString() + "\n";		// Send list of users. What if there are multiple operators?
	channelMsg += "366 " + nick + " " + channelName + " :End of /NAMES list\n";

	send(sockFd, channelMsg.c_str(), channelMsg.length(), 0);
}

void    Server::privMsg(const std::string &msg, const int &sockFd) {
	// If nick not found Exception
	// See codes - 412, 717, 
	std::string sendMsg;

	if (msg.find('#') != std::string::npos) {
		std::string channelName = msg.substr(msg.find('#'), msg.find(':') - msg.find('#') - 1);

		if (_channelHandler.finder(channelName) != NULL) {
			std::vector<Client *>::iterator	it;
			std::string						client = _clientHandler.finder(sockFd, "")->getNick();
			std::vector<Client *>			users = _channelHandler.finder(channelName)->getUsers();

			sendMsg = ':' + client + " PRIVMSG " + channelName + ' ' + msg.substr(msg.find(':'));
			for (it = users.begin(); it != users.end(); it++) {
				int fd = (*it)->getFd();
				if ((*it)->getNick().compare(client))
					send(fd, sendMsg.c_str(), sendMsg.length(), 0);
			}
		}
	}
	else if (_clientHandler.finder(0, msg.substr(8, msg.find(' ', 8) - 8)) != NULL) {
		int fd = _clientHandler.finder(0, msg.substr(8, msg.find(' ', 8) - 8))->getFd();
		sendMsg = ":" + _clientHandler.finder(sockFd, "")->getNick() + " PRIVMSG ";
		sendMsg += _clientHandler.finder(fd, "")->getNick() + ' ' + msg.substr(msg.find(':'));
		send(fd, sendMsg.c_str(), sendMsg.length(), 0);
	}
}

void    Server::setClientNick(const std::string &msg, const int &sockFd) {
	std::string	nick = msg.substr(5, msg.find('\0', 5));

	nick.erase(nick.find('\r'), 2);
	_clientHandler.finder(sockFd, "")->setNick(nick);
	nick = "001 " + nick + "\n";
	send(sockFd, nick.c_str(), nick.length(), 0);
}

void    Server::setClientUser(const std::string &msg, const int &sockFd) {
	std::string	user = msg.substr(msg.find(':') + 1);

	user.erase(user.find('\r'), 2);
	_clientHandler.finder(sockFd, "")->setUser(user);
}