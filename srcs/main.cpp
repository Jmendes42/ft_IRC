# include "../include/Server.hpp"


int	main() 
{
    Server server(54000, "012");

    server._sockSet();
    server._chat();
	return 0;
}
