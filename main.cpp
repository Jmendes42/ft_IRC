# include "header.hpp"


int	main() {

    Server server(54000, "olaines");

    server._sockSet();
    server._chat();
	return 0;
}
