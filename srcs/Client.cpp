#include "../include/Client.hpp"
#include "../include/Channel.hpp"

Channel *Client::findChannel(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return NULL;
}


void Client::addChannel(Channel *add) {
    if (findChannel(add->getName()))
        std::cout <<  "This Client is already in this Channel!" << std::endl;
    else
        _channels.push_back(add);
}

void Client::rmvChannel(const std::string &name)
{
    if (!findChannel(name))
        std::cout << "THIS CLIENT IS NOT IN THE CHANNEL" << std::endl;
    else 
        _channels.erase(_it);
}