#include "../include/Channel.hpp"

Channel::Channel(std::string const &name, Client *chop) : _name(name), _chop(chop), _topic("NoTopic")
{
    MSG("OLA");
    MSG(_chop->getNick());
    addUser(chop);
    initFlags();
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

//NOT TESTED. For now, Topic can't be changed because <t> flag is always false
void Channel::cmdTopic(std::string const &topic, Client *client)
{
    if (topic.empty())
        MSG("TOPIC = " + _topic);
        //send(client->getFd(), topic.c_str(), topic.length(), 0);
    else 
    {
        std::map<char, bool>::iterator it = _flags.find('t');
        if (!(_chop->getNick().compare(client->getNick())) && (it->second == true))
            setTopic(topic);
        else
            MSG("Flag <t> is false");
    }
}