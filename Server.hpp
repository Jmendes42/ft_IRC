#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include "header.hpp"

class Server
{
    int                     _port;
    Socket                  _sock;
    std::string             _password;

    public:
        Server();
        Server(const int &port, const std::string &password):
            _port(port), _password(password), _sock(Socket(_port)) {};

        void    _chat();
        void    _sockSet();
        void    _interpreter(std::string);
};



void	Server::_chat() {

    int clientSocket = _sock.getClientSocket();
    std::string msg = _sock.getMessage();
	std::string nick;
	std::string user;
	char buf[4096];
	int	counter = 0;

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
	//	send(clientSocket, buf, bytesRecv + 1, 0);				// Resend msg
		if (msg == "/exit") {								// Not worki'n
			MSG("The client has disconnected");
			break;
		}
		send(clientSocket, "001 jmendes\n", 12 + 1, 0);
	}
	close(clientSocket);
}



void    Server::_sockSet() {
    _sock._bind();
	_sock._clientSet();
}


void    Server::_interpreter(std::string msg) {
    std::string cmd = msg.substr(0, msg.find(' '));

}

#endif
