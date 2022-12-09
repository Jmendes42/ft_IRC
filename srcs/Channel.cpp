#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Socket.hpp"
#include "../include/Client.hpp"


Channel::Channel(std::string const &name, Client *chop) : _name(name), _chop(chop), _topic("")
{
    MSG(_chop->getNick());
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

/**
 * @brief Checks if the flag is Simple, PS or Complex
 * @param flag to be checked
**/
int checkFlag(char flag)
{
    if (flag == 'p' || flag == 's')
    {
        std::cout << "FLAG = " << flag << std::endl;
        return (1);
    }
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
    
    temp = (it->first == 'p') ?  _flags.find('s') : _flags.find('p');
    if (set == '+' && it->second == false)
    {
        it->second = true;
        if (temp->second == true)
            temp->second = false;
    }
    else if (set == '-' && it->second == true)
        it->second = false;
    else
        MSG("FLAG already set");
}

/**
 * @brief Set the password of the Channel.
 * @param set control if the function will set or unset the password
 * @param args the new password for the channel.
**/
void Channel::changePassword(char set, std::string const &args)
{
    if (set == '-')
        _password.clear();
    else if (!args.empty())
        _password = args;
    else
        MSG("ERROR: MISSING ARGS TO SET THE PASSWORD OF THE CHANNEL");
}

/**
 * @brief Set the User limit of the Channel.
 * @param set control if the function will set or unset the limit 
 * @param args the new password for the channel.
**/
void Channel::setLimit(char set, std::string const &args)
{
    std::cout << "Pass = " << _user_limit << std::endl;
    if (set == '-')
        _user_limit = 0;
    else if (!args.empty())
        _user_limit = atoi(args.c_str());
    else
        MSG("ERROR: MISSING ARGS TO SET THE LIMIT OF THE CHANNEL");
}

int Channel::checkChop(std::string const &nickname)
{
    if (!nickname.compare(_chop->getNick()))
            return 1;
    std::vector<Client *>::iterator it;
    for (it = _sec_chops.begin(); it != _sec_chops.end(); it++)
    {
        if (!((*it)->getNick().compare(nickname)))
            return 1;
    }
    return 0;
}

void Channel::cmdMode(std::string const &flags, std::string const &args, Client *client)
{
    if (!checkChop(client->getNick()))
    {
        std::cout << "ERROR: This client is not a CHOP, can't use MODE command" << std::endl;
        return ;
    }
    char set = flags[0];
    for (int i = 1; i < flags.length(); i++)
    {
        // In case of P or S ! Only one can be set at a time
        if (checkFlag(flags[i]) == 1)
            changeModePS(set, flags[i]);
        else if (checkFlag(flags[i]) == 2)                          
            changeSimpleFlag(set, flags[i]);
        else                                                    
        {
            switch (flags[i])
            {
                case 'o':                                          
                    (set == '+') ? addToVector(args, _sec_chops, _users) : rmvFromVector(args, _sec_chops); 
                    break;
                case 'l':                                           
                    setLimit(set, args);
                    break;
                case 'b':                                       
                    (set == '+') ? addToVector(args, _ban_users, _users) : rmvFromVector(args, _ban_users);
                    break;
                case 'v':                                      
                    (set == '+') ? addToVector(args, _muted_users, _users) : rmvFromVector(args, _muted_users);
                    break;
                case 'k':                                   
                    changePassword(set, args);
                    break;               
                default:
                    send(client->getFd(), "672 flags :Unknown Flags\n", 26, 0); 
                    break;
            }
        }
    }
}

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

// LOOK AT CODES: 377, 470, 485, 495
void Channel::cmdKick(std::string const &nickname, Client *client)
{
    std::string msgSend;
    std::vector<Client *>::iterator it_clients;

    for (it_clients = _users.begin(); it_clients != _users.end(); it_clients++)
    {
        if (!((*it_clients)->getNick().compare(nickname)))
        {
            msgSend =":" + client->getNick() + " KICK " + getName() + ' ' + (*it_clients)->getNick() + '\n';
            send((*it_clients)->getFd(), msgSend.c_str(), msgSend.length(), 0);
            return ;
        }
    }
    std::cout << "ERROR: This User is not in the Channel" << std::endl;
}

void        Channel::cmdInvite(Client *client, Client *toInv)
{
    std::string msgSend;
    msgSend =":" + client->getNick() + " INVITE " + toInv->getNick() + ' ' + getName() + '\n';
    send(toInv->getFd(), msgSend.c_str(), msgSend.length(), 0);
}