#ifndef CLIENT_HANDLER_HPP
# define CLIENT_HANDLER_HPP

# include "Client.hpp"

class ClientHandler
{
    private:

        std::vector<Client *> _clients;
        
    public:

        ClientHandler() {};
        ~ClientHandler() {};

        void    rmvClient(std::string const &);
        void    privateMsg(std::string const &);
        void    editClient(std::vector<std::string> &, int);
        void    addClient(std::string const &, std::string const &);
};

#endif