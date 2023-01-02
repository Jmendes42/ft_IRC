#include "../include/Channel.hpp"
#include "../include/ChannelHandler.hpp"
#include "../include/Socket.hpp"
#include "../include/Macros.hpp"

void    ChannelHandler::addChannel(std::string const &channelName, Client *chop)
{
    _channels.push_back(new Channel(channelName, chop));
}

// ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
// RPL_NOTOPIC                     RPL_TOPIC
// ERR_CHANOPRIVSNEEDED
void ChannelHandler::opTopic(std::vector<std::string> info, Client *client) {
    
    if (info[2].empty())
        finder(info[1])->sendTopic(client);
    else
        finder(info[1])->cmdTopic(info[2], client);
}



Channel *ChannelHandler::finder(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return (NULL);
}

void    ChannelHandler::rmvChannel(std::string const &name) {
    finder(name);
    ERASE_VEC(_channels, _it);
}
