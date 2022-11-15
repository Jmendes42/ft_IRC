
#ifndef CLIENT_HANDLER_HPP
# define CLIENT_HANDLER_HPP

# include <iostream>
#include  <vector>
# include "Client.hpp"


class ClientHandler
{
    private:

        std::vector<Client *> _clients;

    public:

        void    editClient(std::vector<std::string> &info);
        void    addClient(std::string const &msg);
        void    rmvClient(std::string &msg);

};

void    ClientHandler::editClient(std::vector<std::string> &info)
{
    _clients.back()->setUser(info[1]);
    _clients.back()->setHost(info[2]);
    _clients.back()->setReal(info[4]);
}

void    ClientHandler::addClient(std::string const &msg)
{
    std::vector<std::string> info;
    int pos = 0;
    int start = 0;
    while ((pos = msg.find(" ", start)) > -1)
    {
        info.push_back(msg.substr(start, pos - start));
        start = pos + 1;
    }
    pos = msg.length();
    info.push_back(msg.substr(start, pos));

    if (!info[0].compare("NICK"))
        _clients.push_back(new Client(info[1]));
    else if (!info[0].compare("USER"))
        editClient(info);
}

#endif