#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/ClientHandler.hpp"

Client *ClientHandler::finder(const std::string &nick) {
    for (_it = _clients.begin(); _it != _clients.end(); _it++) {
        if (!(*_it)->getNick().compare(nick))
            return (*_it);
    }
    return NULL;
}

Client *ClientHandler::finder(const int &fd) {
    for (_it = _clients.begin(); _it != _clients.end(); _it++) {
        if ((*_it)->getFd() == fd)
            return (*_it);
    }
    return NULL;
}

void    ClientHandler::addClient(std::string const &msg, std::string const &pass, const int &fd,
                                    const std::string &ip)
{
    // Should it look for equal clients?
    std::string msgPass = msg.substr(msg.find(' ') + 1, msg.find(' ') - 1);

    if (!msgPass.compare(pass))
        _clients.push_back(new Client(fd, ip));
    else {
        MSG("Error: Wrong password!");
        exit(0);                                        // Missing Exception
    }
}

// REMOVE BY NICK ! CHECK THE COMMAND TO REMOVE USER TO CHECK WHAT IS NEEDED
void    ClientHandler::rmvClient(std::string const &msg)
{
    for (_it = _clients.begin(); _it != _clients.end(); _it++)
    {
        if (!((*_it)->getNick().compare(msg)))
            _clients.erase(_it);
    }
}

