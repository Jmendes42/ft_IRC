
#include "Client.hpp"
#include "Channel.hpp" // ?? check this include

void Client::addChannel(Channel *add)
{
    std::vector<Channel *>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
    {
        //  THIS SHOULD BE AN EXCEPTION
        if (!((*it)->getName().compare(add->getName())))
        {
            std::cout <<  "This Client is already in this Channel!" << std::endl;
            return ;                                
        }
    }
    _channels.push_back(add);
}

void Client::rmvChannel(std::string &name)
{
    std::vector<Channel *>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
    {
        //  THIS SHOULD BE AN EXCEPTION
        if (!((*it)->getName().compare(name)))
        {
            _channels.erase(it);
            return ;                               
        }
    }
    // THIS SHOULD BE AN EXCEPTION
    std::cout << "THIS CLIENT IS NOT IN THE CHANNEL" << std::endl;
}