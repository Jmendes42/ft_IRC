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


// TOPIC #tardiz :ola
void    ChannelHandler::opCommands(std::string const &msg, Client *chop, chopCommand cmd)
{

    std::vector<std::string> info = ft_split(msg);

    if (info.size() == 3)
        info[2].erase(0, 1);

    Channel *channel = findChannel(info[1]);

    switch(cmd)
    {
        case KICK:
        {
            std::cout << "KICK\n";   
            break;
        }
        case MODE:
        {
            std::string args = "";
            for (int i = 3; i < info.size(); i++)
                args += info[i] + " ";
            channel->cmdMode(info[2], args, chop);
            break;
        }
        case INVITE:
        {
            std::cout << "INVITE\n";   
            break;
        }
        case TOPIC:
        {
            if (info.size() == 3)
            {
                channel->cmdTopic(info[2], chop);
            }
            else
                channel->cmdTopic("", chop);
            break;
        }
    }

}

// UTIL ChannelHandler
Channel *ChannelHandler::findChannel(std::string const &name)
{
	std::vector<Channel *>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
    {
        if (!((*it)->getName().compare(name)))
        {
            return (*it);
        }
    }
    return NULL;
}
