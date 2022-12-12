#include "../include/Channel.hpp"
#include "../include/ChannelHandler.hpp"
#include "../include/Socket.hpp"


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
    std::vector<std::string> info = ft_split(msg);
    
    if (msg.find(':') == std::string::npos)
        finder(info[1])->sendTopic(client);
    else
        finder(info[1])->cmdTopic(msg.substr(msg.find(':')), client);
}

void ChannelHandler::opMode(std::string const &msg, Client *chop)
{
    std::vector<std::string> info = ft_split(msg);
    Channel *channel = finder(info[1]);
    if (!channel)
    {
        std::string toSend =  "403 " +  info[1] + " :No such channel\r\n";
        send(chop->getFd(), toSend.c_str(), toSend.length(), 0);
        return ;
    }
    if (info.size() == 2)
    {
        std::string toSend =  "461 MODE :Not enough parameters\r\n";
        send(chop->getFd(), toSend.c_str(), toSend.length(), 0);
        return ;
    }
    std::string args = "";
    for (int i = 3; i < info.size(); i++)
        args += info[i] + " ";
    args.pop_back();
    channel->cmdMode(info[2], args, chop);
}

//weechat automatically adds the channel name if missing. Also, it warns the user if missing parameters, so there is no way for us to test the errors here.
void ChannelHandler::opKick(const std::string &msg, const std::string &chop, int fd) {
    std::vector<std::string> info = ft_split(msg);
    if (info.size() >= 2)
    {
        Channel *channel = finder(info[1]);
        if (!channel)
        {
            std::string toSend =  "403 " +  info[1] + " :No such channel\r\n";
            send(fd, toSend.c_str(), toSend.length(), 0);
            return ;
        }
        if (info.size() > 2)
        {
            channel->cmdKick(info[2], chop, fd);
            return ;
        }
    }
    
    std::string toSend =  "461 MODE :Not enough parameters\r\n";
    send(fd, toSend.c_str(), toSend.length(), 0);
    return ;
}

Channel *ChannelHandler::finder(const std::string &channelName) {
    for (_it = _channels.begin(); _it != _channels.end(); _it++) {
        if (!(*_it)->getName().compare(channelName))
            return (*_it);
    }
    return (NULL);
}