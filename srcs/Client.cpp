#include "../include/Client.hpp"
#include "../include/Channel.hpp"

Channel *Client::findChannel(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return NULL;
}


bool Client::addChannel(Channel *add) {
    if (findChannel(add->getName()) != NULL) {                                  // Exception
        std::cout <<  "This Client is already in this Channel!" << std::endl;
        return false;
    }
    _channels.push_back(add);
    return true;
}

void Client::rmvChannel(const std::string &name)
{
    for (_it = _channels.begin(); _it != _channels.end(); _it++)
    {
        //  THIS SHOULD BE AN EXCEPTION
        if (!((*_it)->getName().compare(name)))
        {
            _channels.erase(_it);
            return ;                               
        } 
    }
    // THIS SHOULD BE AN EXCEPTION
    std::cout << "THIS CLIENT IS NOT IN THE CHANNEL" << std::endl;
}