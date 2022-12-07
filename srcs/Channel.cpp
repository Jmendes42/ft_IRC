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

//NOT TESTED!!

/**
 * @brief Add User to the vector of Secundary Chops. Checks if the User exists in the Channel and if it is not a Chop already. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be added. 
**/
void    Channel::addChopp(std::string const &nickname)
{
    std::vector<Client *>::iterator it_chop;
    std::vector<Client *>::iterator it_clients;

    for (it_chop = _sec_chops.begin(); it_chop != _sec_chops.end(); it_chop++)
    {
        if (!((*it_chop)->getNick().compare(nickname)))
        {
            std::cout << "ERROR: This chop is already in the Channel" << std::endl;
            return ;
        }
    }
    for (it_clients = _users.begin(); it_clients != _users.end(); it_clients++)
    {
        if (!((*it_clients)->getNick().compare(nickname)))
        {
            _sec_chops.push_back((*it_clients));
            return ;
        }
    }
    std::cout << "ERROR: This User is not in the Channel" << std::endl;
}

/**
 * @brief Remove User from the vector of Secundary Chops., if it exists. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be removed. 
**/
void    Channel::rmvChop(std::string const &nickname)
{
    std::vector<Client *>::iterator it;
    for (it = _sec_chops.begin(); it != _sec_chops.end(); it++)
    {
        if (!((*it)->getNick().compare(nickname)))
        {
            _sec_chops.erase(it);
            return ;
        }
    }
    std::cout << "ERROR: This User is not a CHOPP" << std::endl;
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



// SIMPLE FLAGS
// p - private channel flag;
// s - secret channel flag;
// i - invite-only channel flag;
// t - topic settable by channel operator only flag;
// n - no messages to channel from clients on the outside;
// m - moderated channel;

int checkFlag(char flag)
{
    if (flag == 'p' || flag == 's')
        return (1);
    else if (flag == 'i' || flag == 't' || flag == 'n' || flag == 'm')
        return (2);
    else if (flag == 'o' || flag == 'l' || flag == 'b' || flag == 'v' || flag == 'k')
        return (3);
    return (0);
}

/**
 * @brief Change the state of "simple flags". What is true, will be false and vice-versa. A warning will be throwed if flag was already set.
 * @param set control if the function will set the flag to true or false (+/-)
 * @param flag the flag of the channel that will change state
**/
void Channel::changeSimpleFlag(char set, char flag)
{
    std::map<char, bool>::iterator it = _flags.find(flag);
    if (set == '+' && it->second == false)
        it->second = true;
    else if (set == '-' && it->second == true)
        it->second = false;
    else
        MSG("FLAG already set");
}

/**
 * @brief Change the state of '-p' or '-s' flag. Notice that both flags can't be 'true' at the same time. If you set one to true, the other will be set to false. More info in README
 * @param set control if the function will set the flag to true or false (+/-)
 * @param flag the flag of the channel that will change state
**/
void Channel::changeModePS(char set, char flag)
{
    std::map<char, bool>::iterator it = _flags.find(flag);
    std::map<char, bool>::iterator temp;
    (it->second == 'p') ? temp = _flags.find('s') : temp = _flags.find('p');
    if (set == '+' && it->second == false)
    {
        it->second = true;
        if (temp->second == true)
            temp->second = false;
    }
    else if (set == '-' && it->second == true)
    {
        it->second = false;
        if (temp->second == false)
            temp->second = true;
    }
    else
        MSG("FLAG already set");
}

void Channel::changePassword(char set, std::string const &args)
{



}




void Channel::cmdMode(std::string const &flags, std::string const &args, Client *client)
{
    std::vector<Client *>::iterator it;
    for (it = _sec_chops.begin(); it != _sec_chops.end(); it++)
    {
        if (!((*it)->getNick().compare(client->getNick())))
        {
            _sec_chops.erase(it);
            return ;
        }
    }
    std::cout << "ERROR: This User is not a CHOPP" << std::endl;


    if (flags.empty())
    {
        MSG("No flags for MODE Command");
        return ;
    }
    char set = flags[0];
    for (int i = 1; i < flags.length(); i++)
    {
        if (checkFlag(flags[i]) == 1)                               // In case of P or S ! Only one can be set at a time
            changeModePS(set, flags[i]);
        else if (checkFlag(flags[i]) == 2)                          // In case of "simple" flags, just change state of flag
            changeSimpleFlag(set, flags[i]);
        else                                                        // In case it needs to use args to change state of channel
        {
            switch (flags[i])
            {
                case 'o':                                           // Give/Take channel operator privileges
                    (set == '+') ? addChopp(args) : rmvChop(args); 
                    break;
                case 'l':                                           // Set the user limit to the Channel
                    break;
                case 'b':                                           // Set a ban mask to keep users out
                    break;
                case 'v':                                           // give/take the ability to speak on a moderated channel;
                    break;
                case 'k':                                           // set a channel key (password).
                    break;               
                default:
                    send(client->getFd(), "672 flags :Unknown Flags\n", 26, 0); 
                    break;
            }
        }
    }
}


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