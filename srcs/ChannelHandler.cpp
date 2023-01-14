#include "../include/Channel.hpp"
#include "../include/ChannelHandler.hpp"
#include "../include/Socket.hpp"
#include "../include/Macros.hpp"

void    ChannelHandler::addChannel(std::string const &channelName, Client *chop)
{
    _channels.push_back(new Channel(channelName, chop));
}


Channel *ChannelHandler::finder(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return (NULL);
}

void    ChannelHandler::rmvChannel(std::string const &name) {
    if (finder(name)) 
        ERASE_DEL_VEC(_channels, _it)
}
