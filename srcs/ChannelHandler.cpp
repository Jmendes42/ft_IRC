#include "../include/Channel.hpp"
#include "../include/ChannelHandler.hpp"
#include "../include/Socket.hpp"
#include "../include/Macros.hpp"

void    ChannelHandler::addChannel(std::string const &channelName, Client *chop)
{
    _channels.push_back(new Channel(channelName, chop));
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
    // THIS SHOULD BE AN EXCEPTION Exception
    std::cout << "THIS CLIENT IS NOT IN THE CHANNEL" << std::endl;
}

void ChannelHandler::opTopic(std::string const &msg, Client *client) {
    std::vector<std::string> info = ft_split(msg, ' ');
    
    if (msg.find(':') == std::string::npos)
        finder(info[1])->sendTopic(client);
    else
        finder(info[1])->cmdTopic(msg.substr(msg.find(':')), client);
}

void ChannelHandler::opMode(std::string const &msg, Client *chop)
{
    std::vector<std::string> info = ft_split(msg, ' ');
    std::string toSend;
    // info[1] seg fault
    Channel *channel = finder(info[1]);
    if (!channel)
        ERR_NOSUCHCHANNEL(info[1], chop->getFd(), toSend);
    if (info.size() == 2)
        ERR_NEEDMOREPARAMS(std::string("MODE"), chop->getFd(), toSend)
    std::string args = "";
    for (int i = 3; i < info.size(); i++)
        args += info[i] + " ";
    args.pop_back();
    channel->cmdMode(info[2], args, chop);
}

Channel *ChannelHandler::finder(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return (NULL);
}