
#include "ClientHandler.hpp"

int main()
{
    ClientHandler handle;

    handle.addClient("NICK isousa");
    handle.addClient("USER isousa 0 * :isousa");
        handle.addClient("NICK jmendes");
    handle.addClient("USER jmendes 0 * :jmendes");

    handle.toTest();


    return (0);
}