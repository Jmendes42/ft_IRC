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

void    ClientHandler::addClient(const int &fd, const std::string &ip, const uint16_t &port)
{
    // std::string msgPass = msg.substr(msg.find(' ') + 1, msg.find(' ') - 1);

    // if (!msg.compare(pass))
        _clients.push_back(new Client(fd, ip, port));
    // else {
    //     MSG("Error: Wrong password!");
    //     exit(0);                                        // Missing Exception
    // }
}

void    ClientHandler::rmvClient(std::string const &rmv) {
    if (finder(rmv))
        ERASE_DEL_VEC(_clients, _it);
}

std::string  ClientHandler::processCmd(const std::string &msg, int const &sockFd)
{
	Client		*client = finder(sockFd);

    if (msg.find('\n', 0) == std::string::npos) // in case there is no \n 
    {
        MSG("First: .");

        std::string newCmd = client->getCmdBuffer() + msg;
        client->setCmdBuffer(newCmd);
        return ("");
    }
    if (client->getCmdBuffer().empty())     // in case it is weechat client or nc with \n
        return (msg);
    else
    {
        MSG("Second: ." + client->getCmdBuffer() + ".");

        std::string newCmd = client->getCmdBuffer() + msg;
        MSG("Second NEW: ." + newCmd + ".");

        if (newCmd[newCmd.length() - 1] == '\n')
        {
            client->setCmdBuffer("");
            return (newCmd);
        }
        client->setCmdBuffer(newCmd.substr(newCmd.find_last_of('\n'), newCmd.length() - newCmd.find_last_of('\n')));
        return (newCmd.substr(0, newCmd.find_last_of('\n') - 1));
    }
    return ("");
}
