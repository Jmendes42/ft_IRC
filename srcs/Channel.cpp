#include "../include/Channel.hpp"

Channel::Channel(std::string const &name, Client *chop) : _name(name), _chop(chop)
{
    addUser(chop);
    initFlags();
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


//NOT TESTED!!! 
//This Flags are needed to implement the MODE command
void Channel::initFlags()
{
    _flags.insert(std::pair<char, bool>('o', false));
    _flags.insert(std::pair<char, bool>('p', false));
    _flags.insert(std::pair<char, bool>('s', false));
    _flags.insert(std::pair<char, bool>('i', false));
    _flags.insert(std::pair<char, bool>('t', false));
    _flags.insert(std::pair<char, bool>('n', false));
    _flags.insert(std::pair<char, bool>('m', false));
    _flags.insert(std::pair<char, bool>('l', false));
    _flags.insert(std::pair<char, bool>('b', false));
    _flags.insert(std::pair<char, bool>('v', false));
    _flags.insert(std::pair<char, bool>('k', false));
}

//NOT TESTED!!! Need merge for getFD()
// void Channel::cmdKick(std::string const &topic, Client *client)
// {

// }
// void Channel::cmdMode(std::string const &topic, Client *client)
// {

// }
// void Channel::cmdInvite(std::string const &topic, Client *client)
// {

// }

void Channel::cmdTopic(std::string const &topic, Client *client)
{
    if (topic.empty())
        send(client->getFd(), topic.c_str(), topic.length(), 0);
    std::map<char, bool>::iterator it = _flags.find('t');
    if (_chop == client && (it->second == true))
        setTopic(topic);

}