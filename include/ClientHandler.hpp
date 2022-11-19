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

        void    rmvClient(std::string const &msg);
        //void    prvtMessage();
        void    editClient(std::vector<std::string> &info, int control);
        void    addClient(std::string const &msg, std::string const &pass);
};

#endif