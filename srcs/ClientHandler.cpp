#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/ClientHandler.hpp"


Client *ClientHandler::finder(const std::string &nick)
{
    for (_it = _clients.begin(); _it != _clients.end(); _it++)
    {
        if (!(*_it)->getNick().compare(nick))
            return (*_it);
    }
    return NULL;
}

Client *ClientHandler::finder(const int &fd)
{
    for (_it = _clients.begin(); _it != _clients.end(); _it++)
    {
        if ((*_it)->getFd() == fd)
            return (*_it);
    }
    return NULL;
}

Client *ClientHandler::finder(const uint16_t &port)
{
    for (_it = _clients.begin(); _it != _clients.end(); _it++)
    {
        if ((*_it)->getPort() == port)
            return (*_it);
    }
    return NULL;
}

void    ClientHandler::rmvClient(std::string const &rmv)
{
    if (finder(rmv))
        ERASE_DEL_VEC(_clients, _it);
}

/**
 * @brief  
*/
std::string  ClientHandler::processCmd(const std::string &msg, int const &sockFd)
{
	Client		*client = finder(sockFd);
    std::string newCmd = client->getCmdBuffer() + msg;

    if (msg.find('\n', 0) == std::string::npos) // in case there is no \n 
    {
        client->setCmdBuffer(newCmd);
        return ("");
    }
    if (client->getCmdBuffer().empty())     // in case it is weechat client or nc with \n
        return (msg);
    if (newCmd[newCmd.length() - 1] == '\n')
    {
        client->setCmdBuffer("");
        return (newCmd);
    }
    client->setCmdBuffer(newCmd.substr(newCmd.find_last_of('\n'), newCmd.length() - newCmd.find_last_of('\n')));
    return (newCmd.substr(0, newCmd.find_last_of('\n') - 1));
}
