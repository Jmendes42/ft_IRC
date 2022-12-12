#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Socket.hpp"
#include "../include/Client.hpp"
#include "../include/Macros.hpp"


Channel::Channel(std::string const &name, Client *chop) : _name(name), _topic("")
{
    addChop(chop);
    initFlags();
}

Client  *Channel::finder(std::vector<Client *> &vec, const std::string &nick) {
    for (_it = vec.begin(); _it != vec.end(); _it++) {
        if (!(*_it)->getNick().compare(nick))
            return (*_it);
    }
    return NULL;
}

/**
 * @brief   Get a string with the nicks of all the clients present in the channel with the respective indicator
**/
std::string Channel::getUsersString() {
    std::string users;

    for (_it = _sec_chops.begin(); _it != _sec_chops.end(); _it++)
        users += " @" + (*_it)->getNick();
    for (_it = _users.begin(); _it != _users.end(); _it++)
        users += " " + (*_it)->getNick();    
    for (_it = _muted_users.begin(); _it != _muted_users.end(); _it++)
        users += " " + (*_it)->getNick();
    return users.erase(0, 1);
}

/**
 * @brief       Send message to every client in channel
 * @param msg   msg to send
**/
void    Channel::sendMsgToUsers(const std::string &msg) {
    for (_it = _sec_chops.begin(); _it != _sec_chops.end(); _it++)
        send((*_it)->getFd(), msg.c_str(), msg.length(), 0);
    for (_it = _users.begin(); _it != _users.end(); _it++)
        send((*_it)->getFd(), msg.c_str(), msg.length(), 0);
    for (_it = _muted_users.begin(); _it != _muted_users.end(); _it++)
        send((*_it)->getFd(), msg.c_str(), msg.length(), 0);
}

/**
 * @brief       Send message to every client in channel excluding one fd
 * @param fd    fd to exclude
 * @param msg   msg to send
**/
void    Channel::sendMsgToUsers(const std::string &msg, const int &fd) {
    for (_it = _sec_chops.begin(); _it != _sec_chops.end(); _it++) {
        if ((*_it)->getFd() == fd)
            continue;
        send((*_it)->getFd(), msg.c_str(), msg.length(), 0);
    }
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if ((*_it)->getFd() == fd)
            continue;        
        send((*_it)->getFd(), msg.c_str(), msg.length(), 0);
    }
    for (_it = _muted_users.begin(); _it != _muted_users.end(); _it++) {
        if ((*_it)->getFd() == fd)
            continue;
        send((*_it)->getFd(), msg.c_str(), msg.length(), 0);
    }
}

void    Channel::addChop(Client *chop) {
    if (!finder(_sec_chops, chop->getNick()))
        _sec_chops.push_back(chop);
}

void    Channel::addUser(Client *user) {
    if (!finder(_users, user->getNick()))
        _users.push_back(user);
}

void    Channel::addMute(Client *mute) {
    if (finder(_users, mute->getNick())) {
        rmvClient(mute->getNick());
        _muted_users.push_back(mute);
    }
}

/**
 * @brief       Searches all the client groups for the nick and removes it from the channel vectors
 * @param nick  Nick to search 
 */
void    Channel::rmvClient(const std::string &nick) {
    if (finder(_users, nick))
        _users.erase(_it);
    if (finder(_sec_chops, nick))
        _sec_chops.erase(_it);
    if (finder(_muted_users, nick))
        _muted_users.erase(_it);
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
void Channel::changeSimpleFlag(int fd, char set, char flag, std::string const &channel_name)
{
    std::string toSend;
    std::map<char, bool>::iterator it = _flags.find(flag);
    if (set == '+' && it->second == false)
        it->second = true;
    else if (set == '-' && it->second == true)
        it->second = false;
    else
        ERR_KEYSET(channel_name, fd, toSend);
}

/**
 * @brief Change the state of '-p' or '-s' flag. Notice that both flags can't be 'true' at the same time. If you set one to true, the other will be set to false. More info in README
 * @param set control if the function will set the flag to true or false (+/-)
 * @param flag the flag of the channel that will change state
**/
void Channel::changeModePS(int fd, char set, char flag, std::string const &channel_name)
{
    std::map<char, bool>::iterator it = _flags.find(flag);
    std::map<char, bool>::iterator temp;
    std::string toSend;
    
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
        ERR_KEYSET(channel_name, fd, toSend);
}

/**
 * @brief Set the password of the Channel.
 * @param set control if the function will set or unset the password
 * @param args the new password for the channel.
**/
void Channel::changePassword(int fd, char set, std::string const &args)
{
    std::map<char, bool>::iterator it = _flags.find('k');
    std::string toSend;

    if (set == '-')
    {
        _password.clear();
        if (it->second == true)
            it->second == false;
    }
    else if (set == '-' && !args.empty())
    {
        _password = args;
        if (it->second == false)
            it->second = true;
    }
    else
        ERR_NEEDMOREPARAMS("MODE", fd, toSend);
}

/**
 * @brief Set the User limit of the Channel.
 * @param set control if the function will set or unset the limit 
 * @param args the new password for the channel.
**/
void Channel::setLimit(int fd, char set, std::string const &args)
{
    std::map<char, bool>::iterator it = _flags.find('l');
    std::string toSend;

    if (set == '-')
    {
        _user_limit = 0;
        if (it->second == true)
            it->second == false;
    }
    else if (set == '+' && !args.empty())
    {
        _user_limit = atoi(args.c_str());
        if (it->second == false)
            it->second = true;
    }
    else
        ERR_NEEDMOREPARAMS("MODE", fd, toSend);
}

void Channel::cmdMode(std::string const &flags, std::string const &args, Client *client)
{
    std::string toSend;
    if (!finder(_users, client->getNick()))
        ERR_NOTONCHANNEL(getName(), client->getFd, toSend);
    if (!finder(_sec_chops, client->getNick()))
        ERR_CHANOPRIVSNEEDED(getName(), client->getFd, toSend);
    char set = flags[0];
    for (int i = 1; i < flags.length(); i++)
    {
        // In case of P or S ! Only one can be set at a time
        if (checkFlag(flags[i]) == 1)
            changeModePS(client->getFd(), set, flags[i], getName());
        else if (checkFlag(flags[i]) == 2)                          
            changeSimpleFlag(client->getFd(), set, flags[i], getName());
        else                                                    
        {
            switch (flags[i])
            {
                case 'o':                                          
                    (set == '+') ? addToVector(client->getFd(), getName(), args, _sec_chops, _users) : rmvFromVector(client->getFd(), getName(), args, _sec_chops); 
                    break;
                case 'l':                                           
                    setLimit(client->getFd(), set, args);
                    break;
                case 'b':                                       
                    (set == '+') ? addToVector(client->getFd(), getName(), args, _ban_users, _users) : rmvFromVector(client->getFd(), getName(), args, _ban_users);
                    break;
                case 'v':                                      
                    (set == '+') ? addToVector(client->getFd(), getName(), args, _muted_users, _users) : rmvFromVector(client->getFd(), getName(), args, _muted_users);
                    break;
                case 'k':                                   
                    changePassword(client->getFd(), set, args);
                    break;               
                default:
                    ERR_UNKNOWNMODE(getName(), client->getFd, toSend);
            }
        }
    }
}

void Channel::sendTopic(Client *client) {
    std::string msg;

    if (_topic.empty()) {
        msg = "331 " + client->getNick() + ' ' + _name + '\n';
        send(client->getFd(), msg.c_str(), msg.length(), 0);     // NOTOPICSET
    }
    else {
        msg = "332 " + client->getNick() + ' ' + _name + ' ' + _topic + '\n';
        send(client->getFd(), msg.c_str(), msg.length(), 0);     // TOPICSET
    }
}

void Channel::cmdTopic(const std::string &topic, Client *client) {
        //std::map<char, bool>::iterator it = _flags.find('t');  && (it->second == true)
        std::string toSend;
        if (finder(_sec_chops, client->getNick())) {
            setTopic(topic);
            sendTopic(client);
        }
        else
            ERR_CHANOPRIVSNEEDED(getName(), client->getFd(), toSend);
}

// LOOK AT CODES: 377, 470, 485, 495
void Channel::cmdKick(std::string const &nickname, const std::string &kicker, int fd) {
    std::string msgSend;

    if (!finder(_users, kicker))
        ERR_NOTONCHANNEL(getName(), fd, msgSend);
    if (!finder(_sec_chops, kicker)) {
        ERR_CHANOPRIVSNEEDED(getName(), fd, msgSend);
    if (finder(_users, nickname)) {                                                     // See muted
        msgSend =":" + kicker + " KICK " + _name + ' ' + nickname + '\n';
        sendMsgToUsers(msgSend);
        rmvClient(nickname);
        return ;
    }
    ERR_NOSUCHNICK(getName(), fd, msgSend);
}

void        Channel::cmdInvite(Client *client, Client *toInv) {
    std::string msgSend;
    
    msgSend =":" + client->getNick() + " INVITE " + toInv->getNick() + ' ' + getName() + '\n';
    send(toInv->getFd(), msgSend.c_str(), msgSend.length(), 0);
}

/**
 * @brief Leave the Channel (using PART Command)
 * @param client a Pointer to the client that wants to leave the Channel
**/
void Channel::partChannel(Client *client)
{
    std::string msgSend;

    if (finder(_users, client->getNick()) || finder(_sec_chops, client->getNick())) {
        msgSend = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getIp() + " PART :" + _name + "\r\n";
        sendMsgToUsers(msgSend);
        rmvClient(client->getNick());
        return ;
    }
    ERR_NOSUCHCHANNEL(_name, client->getFd(), msgSend);
}

bool Channel::retStateFlag(char flag)
{
    std::map<char, bool>::iterator it = _flags.find(flag);
    return (it->second);
}

bool Channel::checkBan(std::string const &nick)
{
    if (!finder(_ban_users, nick))
        return (false);
    return (true);
}

