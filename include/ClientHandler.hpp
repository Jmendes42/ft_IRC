#ifndef CLIENT_HANDLER_HPP
# define CLIENT_HANDLER_HPP

# include <vector>
# include <string>
# include <iostream>

# include "Client.hpp"
# include "Socket.hpp"

class Client;

class ClientHandler
{
    private:

        std::vector<Client *>::iterator _it;
        std::vector<Client *>           _clients;
        
    public:

        ClientHandler() {};
        ~ClientHandler() {};

        void    rmvClient(std::string const &);
        void    addClient(std::string const &, std::string const &, const int &,
                            const std::string &);

        Client * finder(const int &);
        Client * finder(const std::string &);

        std::vector<Client *>   &getClients()   {return _clients;};
};

#endif