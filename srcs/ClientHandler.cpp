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

Client *ClientHandler::finder(const uint16_t &port) {
    for (_it = _clients.begin(); _it != _clients.end(); _it++) {
        if ((*_it)->getPort() == port)
            return (*_it);
    }
    return NULL;
}

void    ClientHandler::addClient(std::string const &msg, std::string const &pass, const int &fd,
                                    const std::string &ip, const uint16_t &port)
{
    std::string msgPass = msg.substr(msg.find(' ') + 1, msg.find(' ') - 1);

    if (!msgPass.compare(pass))
        _clients.push_back(new Client(fd, ip, port));
    else {
        MSG("Error: Wrong password!");
        exit(0);                                        // Missing Exception
    }
}

void    ClientHandler::rmvClient(std::string const &rmv) {
    if (finder(rmv))
        ERASE_VEC(_clients, _it);
}

