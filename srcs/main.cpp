# include "../include/Server.hpp"
#include <sstream>
#include <string>


int	main(int argc, char **argv) 
{
    int port;
    std::string password;
 
    // REPLACE THIS WITH AN ERROR MESSAGE (Just here for testing purpose)
    if (argc != 3)
    {
        port = 54000;
        password = "012";
    }
    else
    {
        std::istringstream(argv[1]) >> port; //i is 10 after this
        password = argv[2];
    }
    std::cout << "Port = " << port << " | Pass = " << password << std::endl;

    Server server(port, password);

   /* server._sockSet();
    int bytesRecv;
    int clientSocket = server._sock.getClientSocket();
    std::string msg = server._sock.getMessage();
	char buf[4096];*/

	server._sockSet();
    while (true)
    {
        server._sock.sets();
		// wait for an activity on one of the sockets , timeout is NULL ,
		// so wait indefinitely
		server.activity();
		// Clear the buffer
    }
    // server._chat();
	return 0;
}


    

	// while (true) {

	// 	MSG("Loop");
	// 	// Clear the buffer
	// 	ft_memset(buf, 0, 4096);
	// 	// Wait for a message
	// 	int bytesRecv = recv(clientSocket, buf, 4096, 0);
	// 	if (bytesRecv == -1) {
	// 		MSG("There was a connection issue!");
	// 		break;
	// 	}
	// 	if (bytesRecv == 0) {
	// 		MSG("The client has disconnected");
	// 		break;
	// 	}
	// 	// Client message
	// 	msg = std::string(buf, 0, bytesRecv);
    //     server._interpreter(msg, clientSocket);
	// 	std::cout << "Received: " << msg << std::endl;		// Display msg
	// }
	// close(clientSocket);