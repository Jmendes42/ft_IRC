#include "../include/Server.hpp"

void	Server::_chat() {

    int clientSocket = _sock.getClientSocket();
    std::string msg = _sock.getMessage();
	char buf[4096];

	while (true) {

		// Clear the buffer
		memset(buf, 0, 4096);
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
        _interpreter(msg);
		std::cout << "Received: " << msg << std::endl;		// Display msg
		send(clientSocket, "001 RPL_WELCOME\n", 12 + 1, 0);
	}
	close(clientSocket);
}

void    Server::_sockSet() {
    _sock._bind();
	_sock._clientSet();
}

void    Server::_interpreter(std::string const &msg) {
    std::string cmd = msg.substr(0, msg.find(' '));

	if (!cmd.compare("PASS") || !cmd.compare("NICK") || !cmd.compare("USER"))
		_clientHandler.addClient(msg, _password);
	if (!cmd.compare("PRIVMSG"))
		_clientHandler.privateMsg(msg);
}
