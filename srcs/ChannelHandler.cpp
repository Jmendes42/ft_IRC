#include "../include/ChannelHandler.hpp"

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

void ChannelHandler::opTopic(std::string const &msg, Client *chop)
{
    std::vector<std::string> info = ft_split(msg);
    Channel *channel = finder(info[1]);
    if (info.size() == 3)
    {
        info[2].erase(0, 1);
        channel->cmdTopic(info[2], chop);
    }
    else
        channel->cmdTopic("", chop);
}

void ChannelHandler::opMode(std::string const &msg, Client *chop)
{
    std::vector<std::string> info = ft_split(msg);
    Channel *channel = finder(info[1]);
    if (info.size() == 2)
    {
        MSG("No flags for MODE Command");
        return ;
    }
    std::string args = "";
    for (int i = 3; i < info.size(); i++)
        args += info[i] + " ";
    args.pop_back();
    channel->cmdMode(info[2], args, chop);
}

void ChannelHandler::opKick(std::string const &msg, Client *chop)
{
    std::vector<std::string> info = ft_split(msg);
    Channel *channel = finder(info[1]);
    std::cout << "KICK\n"; 
    channel->cmdKick(info[2], chop);
}

void ChannelHandler::opInvite(std::string const &msg, Client *chop)
{
    std::vector<std::string> info = ft_split(msg);
    Channel *channel = finder(info[1]);
    std::cout << "INVITE\n";   
}



// UTIL ChannelHandler

Channel *ChannelHandler::finder(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return (NULL);
}