#include "../include/ChannelHandler.hpp"

void    ChannelHandler::addChannel(std::string const &msg, Client *chop)
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
    info.push_back(msg.substr(start, pos - start));

    std::vector<Channel *>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
    {
        // THIS SHOULD BE AN EXCEPTION
        if (!((*it)->getName().compare(info[1])))
        {
            (*it)->addUser(chop);
            return ;                                
        }
    }
    _channels.push_back(new Channel(info[1], chop));
}

// REMOVE BY NICK ! CHECK THE COMMAND TO REMOVE USER TO CHECK WHAT IS NEEDED
void    ChannelHandler::rmvClient(std::string const &nick)
{
    std::vector<Channel *>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
    {
        if (!((*it)->getName().compare(nick)))
        {
            _channels.erase(it);
            return ;
        }
    }
    // THIS SHOULD BE AN EXCEPTION
    std::cout << "THIS CLIENT IS NOT IN THE CHANNEL" << std::endl;
}
