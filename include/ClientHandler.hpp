#ifndef CLIENT_HANDLER_HPP
# define CLIENT_HANDLER_HPP

# include "Client.hpp"

class ClientHandler
{
    private:

        std::vector<Client *>::iterator _it;
        std::vector<Client *>           _clients;
        
    public:

        ClientHandler() {};
        ~ClientHandler() {};

        void    rmvClient(std::string const &);
        void    privateMsg(std::string const &);
        void    editClient(std::vector<std::string> &, int);
        void    addClient(std::string const &, std::string const &, const int &);

        Client * finder(const int &, const std::string &);

        std::vector<Client *>           &getClients()   {return _clients;};
};

#endif