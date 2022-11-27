# include "../include/Server.hpp"
#include <sstream>
#include<string>

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
    server._chat();
	return 0;
}