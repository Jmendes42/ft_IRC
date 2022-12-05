#include "../include/Channel.hpp"

Channel::Channel(std::string const &name, Client *chop) : _name(name), _chop(chop), _topic("")
{
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
    _flags.insert(std::pair<char, bool>('t', true));
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


// FLAGS THAT NEED ARGS
// o- give/take channel operator privileges;
// l - set the user limit to channel;
// b - set a ban mask to keep users out;
// v - give/take the ability to speak on a moderated channel;
// k - set a channel key (password).

// SIMPLE FLAGS
// p - private channel flag;
// s - secret channel flag;
// i - invite-only channel flag;
// t - topic settable by channel operator only flag;
// n - no messages to channel from clients on the outside;
// m - moderated channel;

// int checkFlag(std::string const &flags, int i)
// {
//     if (flags[i] == 'p' || flags[i] == 's' || flags[i] == 'i' || flags[i] == 't' || flags[i] == 'n' || flags[i] == 'm')
//         return (1);
//     return (0);
// }

// void Channel::cmdMode(std::string const &flags, std::string const &args, Client *client)
// {
//     if (flags.empty())
//     {
//         MSG("No flags for MODE Command");
//         return ;
//     }
//     char set = flags[0];
//     std::map<char, bool>::iterator it;
//     for (int i = 1; i < flags.length(); i++)
//     {
//         it = _flags.find(flags[i]);
//         if (checkFlag(flags, i))                // In case of "simple" flags, just change state of flag
//         {
//             if (set == '+' && it->second == false)
//                 it->second = true;
//             else if (set == '-' && it->second == true)
//                 it->second = false;
//             else
//                 MSG("FLAG already set"); 
//         }
//         else                                    // In case it needs to use args to change state of channel
//         {
            
//         }

//     }
//     // MSG("Flags = " + flags);

// }


// void Channel::cmdInvite(std::string const &topic, Client *client)
// {

// }

//NOT TESTED. For now, Topic can't be changed because <t> flag is always false
// send(fd,":ines!ines@localhost 413 ines Ola\n" , 35, 0);
void Channel::cmdTopic(std::string const &topic, Client *client)
{
    if (topic.empty())
    {
        if (_topic.empty())
        {
        MSG("OLA");
            send(client->getFd(), "331 isousa #tardiz\n", 20, 0);     // NOTOPICSET

        }
        else
            send(client->getFd(), "332 isousa #tardiz :ola\n", 25, 0);     // TOPICSET
    }
    else 
    {
        std::map<char, bool>::iterator it = _flags.find('t');
        if (!(_chop->getNick().compare(client->getNick())) && (it->second == true))
            setTopic(topic);
        else
            MSG("Flag <t> is false");
    }
}