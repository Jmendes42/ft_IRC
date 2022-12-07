#include "../include/Channel.hpp"

Channel::Channel(std::string const &name, Client *chop) : _name(name), _chop(chop)
{
    addUser(chop);
}

std::string Channel::getUsersString() {
    std::string users;

    for (_it = _users.begin(); _it != _users.end(); _it++)
        users += " " + (*_it)->getNick();
    return users.erase(0, 1);
}

void    Channel::addUser(Client *user)
{
    for (_it = _users.begin(); _it != _users.end(); _it++)
    {
        if (!((*_it)->getNick().compare(user->getNick())))
        {
            std::cout << "ERROR: This User is already in the Channel" << std::endl; // Exception
            return ;
        }
    }
    _users.push_back(user);
}

void    Channel::rmvUser(std::string const &nickname)
{
    for (_it = _users.begin(); _it != _users.end(); _it++)
    {
        if (!((*_it)->getNick().compare(nickname)))
        {
            _users.erase(_it);
            return ;
        }
    }
}