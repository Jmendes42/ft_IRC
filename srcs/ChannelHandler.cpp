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
void ChannelHandler::opTopic(std::string const &msg, Client *client) {
    std::vector<std::string> info = ft_split(msg, ' ');
    
    if (msg.find(':') == std::string::npos)
        finder(info[1])->sendTopic(client);
    else
        finder(info[1])->cmdTopic(msg.substr(msg.find(':')), client);
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
