#ifndef CLIENT_HANDLER_HPP
# define CLIENT_HANDLER_HPP

# include <vector>
# include <string>
# include <iostream>

# include "Client.hpp"
# include "Socket.hpp"
# include "Macros.hpp"



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
        void    addClient(const int &fd, const std::string &ip, const uint16_t &port);

        Client  *finder(const int &);
        Client  *finder(const uint16_t &);
        Client  *finder(const std::string &);

        std::vector<Client *>   &getClients()   {return _clients;};

        std::string  processCmd(const std::string &msg, int const &sockFd);
};

#endif