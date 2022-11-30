#include "../include/Server.hpp"

void	Server::_chat() {

    int clientSocket = _sock.getClientSocket();
    std::string msg = _sock.getMessage();
	char buf[4096];

	while (true) {

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
}

void    Server::_sockSet() {

	_sock.init_sockets();
    _sock._bind();

	// _sock._clientSet();
}

void    Server::_interpreter(std::string const &msg, int const &clientSocket) {
    std::string cmd = msg.substr(0, msg.find(' '));

	if (!cmd.compare("PASS") || !cmd.compare("NICK") || !cmd.compare("USER")) {

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
