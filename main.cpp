
#include "ClientHandler.hpp"

int main()
{
    ClientHandler handle;

    handle.addClient("NICK isousa");
    handle.addClient("USER isousa 0 * :isousa");
        handle.addClient("NICK jmendes");
    handle.addClient("USER jmendes 0 * :jmendes");
    handle.addClient("NICK ola");
    handle.addClient("USER ola 0 * :ola");
        handle.addClient("NICK adeus");
    handle.addClient("USER adeus 0 * :adeus");

    handle.rmvClient("ola");

    return (0);
}