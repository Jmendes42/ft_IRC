#include "../include/Channel.hpp"

Channel::Channel(std::string const &name, Client *chop) : _name(name), _chop(chop)
{
    addUser(chop);
}

void    Channel::addUser(Client *user)
{
    std::vector<Client *>::iterator it;

    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (!((*it)->getNick().compare(user->getNick())))
        {
            std::cout << "ERROR: This User is already in the Channel" << std::endl;
            return ;
        }
    }
    _users.push_back(user);
}

void    Channel::rmvUser(std::string const &nickname)
{
    std::vector<Client *>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (!((*it)->getNick().compare(nickname)))
        {
            _users.erase(it);
            return ;
        }
    }
}