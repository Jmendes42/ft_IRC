# include "../include/Server.hpp"
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

    server._sockSet();

    while (true)
    {
        server._sock.sets();
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		server._sock.activity();
    }



    // server._chat();
	return 0;
}
