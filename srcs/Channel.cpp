#include "../include/Channel.hpp"

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

//NOT TESTED!!

/**
 * @brief Add User to the vector of Ban users. Checks if the User exists in the Channel and if it is not a Chop already. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be added. 
**/
void    Channel::addMute(std::string const &nickname)
{
    if (nickname.empty())
    {
        std::cout << "ERROR: NO NICKNAME" << std::endl;
        return ;
    }
    std::vector<Client *>::iterator it_chop;
    std::vector<Client *>::iterator it_clients;

    for (it_chop = _muted_users.begin(); it_chop != _muted_users.end(); it_chop++)
    {

        if (!((*it_chop)->getNick().compare(nickname)))
        {
            std::cout << "ERROR: Banned User" << std::endl;
            return ;
        }
    }
    for (it_clients = _users.begin(); it_clients != _users.end(); it_clients++)
    {
        if (!((*it_clients)->getNick().compare(nickname)))
        {
            std::cout << "Banning user" << std::endl;
            _muted_users.push_back((*it_clients));
            return ;
        }
    }
    std::cout << "ERROR: This User is not in the Channel" << std::endl;
}

/**
 * @brief Add User to the vector of Ban users. Checks if the User exists in the Channel and if it is not a Chop already. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be added. 
**/
void    Channel::addBan(std::string const &nickname)
{
    if (nickname.empty())
    {
        std::cout << "ERROR: NO NICKNAME" << std::endl;
        return ;
    }
    std::vector<Client *>::iterator it_chop;
    std::vector<Client *>::iterator it_clients;

    for (it_chop = _ban_users.begin(); it_chop != _ban_users.end(); it_chop++)
    {

        if (!((*it_chop)->getNick().compare(nickname)))
        {
            std::cout << "ERROR: Banned User" << std::endl;
            return ;
        }
    }
    for (it_clients = _users.begin(); it_clients != _users.end(); it_clients++)
    {
        if (!((*it_clients)->getNick().compare(nickname)))
        {
            std::cout << "Banning user" << std::endl;
            _ban_users.push_back((*it_clients));
            return ;
        }
    }
    std::cout << "ERROR: This User is not in the Channel" << std::endl;
}

/**
 * @brief Add User to the vector of Secundary Chops. Checks if the User exists in the Channel and if it is not a Chop already. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be added. 
**/
void    Channel::addChopp(std::string const &nickname)
{
    if (nickname.empty())
    {
        std::cout << "ERROR: NO NICKNAME" << std::endl;
        return ;
    }
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
            std::cout << "Adding CHOP" << std::endl;
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

/**
 * @brief Remove User from the vector of Secundary Chops., if it exists. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be removed. 
**/
void    Channel::rmvBan(std::string const &nickname)
{
    std::vector<Client *>::iterator it;
    for (it = _ban_users.begin(); it != _ban_users.end(); it++)
    {
        if (!((*it)->getNick().compare(nickname)))
        {
            _ban_users.erase(it);
            return ;
        }
    }
    std::cout << "ERROR: This User is not Banned" << std::endl;
}

/**
 * @brief Remove User from the vector of Secundary Chops., if it exists. (CHOP = Channel Operator)
 * @param nickname Nickname of the user to be removed. 
**/
void    Channel::rmvMute(std::string const &nickname)
{
    std::vector<Client *>::iterator it;
    for (it = _muted_users.begin(); it != _muted_users.end(); it++)
    {
        if (!((*it)->getNick().compare(nickname)))
        {
            _muted_users.erase(it);
            return ;
        }
    }
    std::cout << "ERROR: This User is not Banned" << std::endl;
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
// LOOK AT CODES: 377, 470, 485, 495
void Channel::cmdKick(std::string const &nickname, Client *client)
{
    std::vector<Client *>::iterator it_clients;
    std::string msgSend;
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

// SIMPLE FLAGS
// p - private channel flag;
// s - secret channel flag;
// i - invite-only channel flag;
// t - topic settable by channel operator only flag;
// n - no messages to channel from clients on the outside;
// m - moderated channel;

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
                    setLimit(set, args);
                    break;
                case 'b':                                           // Set a ban mask to keep users out
                    (set == '+') ? addBan(args) : rmvBan(args);
                    break;
                case 'v':                                           // give/take the ability to speak on a moderated channel
                    (set == '+') ? addMute(args) : rmvMute(args);
                    break;
                case 'k':                                           // set a channel key (password).
                    changePassword(set, args);
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