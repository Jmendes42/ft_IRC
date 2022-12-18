#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/Socket.hpp"
#include "../include/Client.hpp"
#include "../include/Macros.hpp"
#include "../include/Channel.hpp"

Channel::Channel(std::string const &name, Client *chop) : _name(name), _topic(""), _moderatedChannel(false)
{
    addClient(_chops, chop);
    initFlags();
}

/**
 * @brief           Searches user on the provided vector
 * @param client    User to search for
**/
Client  *Channel::finder(std::vector<Client *> &vec, Client *client) {
    std::string nick = client->getNick();

    for (_it = vec.begin(); _it != vec.end(); _it++) {
        if (!(*_it)->getNick().compare(nick))
            return (*_it);
    }
    return NULL;
}

/**
 * @brief       returns true if the user is on the channel and false if it's not
 * @param nick  nickname to search for
**/
bool    Channel::usersOnChannel(Client *find) {
    if (finder(_users, find) || finder(_chops, find) || finder(_muted_users, find)
            || finder(_moderators, find))
        return true;
    return false;
}

/**
 * @brief   returns true if there's users on the channel and false it's empty
**/
bool    Channel::usersOnChannel() {
    if (_users.empty() && _chops.empty() && _muted_users.empty())
        return false;
    return true;
}

/**
 * @brief   Get a string with the nicks of all the clients present in the channel with the respective indicator
**/
std::string Channel::getUsersString() { // + if the user has the rigth to speak in a moderated channel
    std::string users;

    for (_it = _chops.begin(); _it != _chops.end(); _it++)
        users += " @" + (*_it)->getNick();
    for (_it = _moderators.begin(); _it != _moderators.end(); _it++)
        users += " +" + (*_it)->getNick();
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
    for (_it = _chops.begin(); _it != _chops.end(); _it++)
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
    for (_it = _chops.begin(); _it != _chops.end(); _it++) {
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

/**
 * @brief           Adds user to the invited list(vector)
 * @param client    Client to be invited
 */
void    Channel::addInvited(Client *client) {
    _invited_users.push_back(client);
}

/**
 * @brief       Searches all the client groups for the nick and removes it from the channel vectors
 * @param nick  Nick to search 
 */
void    Channel::rmvClient(Client *rmv) {
    if (finder(_chops, rmv))
        _chops.erase(_it);
    else if (finder(_users, rmv))
        _users.erase(_it);
    else if (finder(_ban_users, rmv))
        _ban_users.erase(_it);
    else if (finder(_moderators, rmv))
        _moderators.erase(_it);
    else if (finder(_muted_users, rmv))
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
 * @brief       Checks if the flag is Simple, PS or Complex
 * @param flag  Flag to be checked
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
 * @brief       Change the state of "simple flags". What is true, will be false and vice-versa. A warning will be throwed if flag was already set.
 * @param set   Control if the function will set the flag to true or false (+/-)
 * @param flag  the flag of the channel that will change state
**/
void Channel::changeSimpleFlag(int fd, char set, char flag, std::string const &channel_name)
{
    std::map<char, bool>::iterator it = _flags.find(flag);
    if (set == '+' && it->second == false)
        it->second = true;
    else if (set == '-' && it->second == true)
        it->second = false;
    else
        ERR_KEYSET(channel_name, fd, _errMsg);
}

/**
 * @brief       Change the state of '-p' or '-s' flag. Notice that both flags can't be 'true' at the same time. If you set one to true, the other will be set to false. More info in README
 * @param set   control if the function will set the flag to true or false (+/-)
 * @param flag  the flag of the channel that will change state
**/
void Channel::changeModePS(int fd, char set, char flag, std::string const &channel_name)
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
        ERR_KEYSET(channel_name, fd, _errMsg);
}

/**
 * @brief       Set the password of the Channel.
 * @param set   Control if the function will set or unset the password
 * @param args  The new password for the channel.
**/
void Channel::changePassword(int fd, char set, std::string const &args)
{
    std::map<char, bool>::iterator it = _flags.find('k');

    if (set == '-')
    {
        _password.clear();
        if (it->second == true)
            it->second = false;
    }
    else if (set == '-' && !args.empty())
    {
        _password = args;
        if (it->second == false)
            it->second = true;
    }
    else
        ERR_NEEDMOREPARAMS(std::string("MODE"), fd, _errMsg);
}

/**
 * @brief       Set the User limit of the Channel.
 * @param set   Control if the function will set or unset the limit 
 * @param args  The new password for the channel.
**/
void Channel::setLimit(int fd, char set, std::string const &args)
{
    std::map<char, bool>::iterator it = _flags.find('l');

    if (set == '-')
    {
        _user_limit = 0;
        if (it->second == true)
            it->second = false;
    }
    else if (set == '+' && !args.empty())
    {
        _user_limit = atoi(args.c_str());
        if (it->second == false)
            it->second = true;
    }
    else
        ERR_NEEDMOREPARAMS(std::string("MODE"), fd, _errMsg);
}

/**
 * @brief           Searches all the client groups for the user, if it exists in any removes it and adds it to the vector provided
 * @param client    Client to search 
 * @param vec       Vector to insert 
 */
void    Channel::addClient(std::vector<Client *> &vec, Client *client) {
    rmvClient(client);
    vec.push_back(client);
}

/**
 * @brief       Checks if the user can be banned, if it can adds it to the banned vector
 * @param user  User to ban 
 * @param flag  Ban flag
 */
void    Channel::banUser(const std::string &flag, Client *user) {
    if (flag[0] == '+' && finder(_ban_users, user)) {
        MSG("User already banned");
        return ;
    }
    else if (flag[0] == '-' && !finder(_ban_users, user)) {
        MSG("User not banned");
        return ;
    }
    (flag[0] == '+') ? addClient(_ban_users, user) : rmvClient(user);
}

/**
 * @brief       Checks if the user can be promoted to chop, if it can adds it to the chops vector
 * @param user  User to promote 
 * @param flag  Chop promote flag
 */
void    Channel::chopMode(const std::string &flag, Client *user) {
    std::string msgSend;

    if (flag[0] == '+' && finder(_chops, user)) {
        MSG("ERR user is already a chop");
        return ;
    }
    if (flag[0] == '-' && !finder(_chops, user)) {
        MSG("ERR user is not a chop");
        return ;
    }
    (flag[0] == '+') ? addClient(_chops, user) : addClient(_users, user);
    msgSend = ":jmendes!jmendes@127.0.0.1 MODE " + _name + " +o " + user->getNick() + "\r\n";
    sendMsgToUsers(msgSend);
}

/**
 * @brief       Checks if the user can be promoted to moderator, if it can adds it to the moderators vector
 * @param user  User to promote 
 * @param flag  Moderator promote flag
 */
void    Channel::moderatorMode(const std::string &flag, Client *user) {
    std::string msgSend;

    if (flag[0] == '+' && finder(_chops, user)) {
        MSG("ERR user is already a chop");
        return ;
    }
    if (flag[0] == '+' && finder(_moderators, user)) {
        MSG("ERR user is already a moderator");
        return ;
    }
    if (flag[0] == '-' && !finder(_moderators, user)) {
        MSG("ERR user is not a moderator");
        return ;
    }
    (flag[0] == '+') ? addClient(_moderators, user) : addClient(_users, user);
    msgSend = ":jmendes!jmendes@127.0.0.1 MODE " + _name + " +v " + user->getNick() + "\r\n";
    //msgSend = "353 " + user->getNick() + " = " + _name + " :" + getUsersString() + "\r\n";
    MSG("MOD -> " + getUsersString() + '+');
    sendMsgToUsers(msgSend);
}

/**
 * @brief           Channels the user mode related flags
 * @param client    Client to promote 
 * @param flag      Chop promote flag
 */
void    Channel::userMode(const std::string &flags, Client *user) {
    if (!usersOnChannel(user))
        ERR_USERNOTINCHANNEL(_name, user->getNick(), user->getFd(), _errMsg);
    if (flags[1] == 'b')
        banUser(flags, user);
    else if (flags[1] == 'o')
        chopMode(flags, user);
    else if (flags[1] == 'v')
        moderatorMode(flags, user);
}

void Channel::cmdMode(std::string const &flags, std::string const &args, Client *chop)
{
    std::string msgSend;

    if (!finder(_users, chop) && !finder(_chops, chop))
        ERR_NOTONCHANNEL(getName(), chop->getFd(), _errMsg);
    if (!finder(_chops, chop))
        ERR_CHANOPRIVSNEEDED(getName(), chop->getFd(), _errMsg);
    char set = flags[0];

    MSG(flags);
    for (long unsigned int i = 1; i < flags.length(); i++) {
        // In case of P or S ! Only one can be set at a time
        if (checkFlag(flags[i]) == 1)
            changeModePS(chop->getFd(), set, flags[i], getName());
        else if (checkFlag(flags[i]) == 2)                          
            changeSimpleFlag(chop->getFd(), set, flags[i], getName());
        else                                                    
        {
            switch (flags[i])
            {
                case 'l':                                           
                    setLimit(chop->getFd(), set, args);
                    break;
                case 'b':                                       
                    (set == '+') ? addToVector(chop->getFd(), getName(), args, _ban_users, _users) : rmvFromVector(chop->getFd(), getName(), args, _ban_users);
                    break;
                case 'k':                                   
                    changePassword(chop->getFd(), set, args);
                    break;               // Apply moderated to private messages and +v flag on users and channels vec of +v
                case 'm':                                       
                    (set == '+') ? _moderatedChannel = true : _moderatedChannel = false;
                    break;               
                default:
                    ERR_UNKNOWNMODE(getName(), chop->getFd(), _errMsg);
            }
        }
    }
}

/**
 * @brief           Prints channel topic to client
 * @param client    Client that requested
 */
void Channel::sendTopic(Client *client) {
    std::string msg;
    std::map<char, bool>::iterator it = _flags.find('t');

    if (it->second == false) {
        MSG("flag ERR");
        return ;
    }
    
    if (_topic.empty()) {
        msg = "331 " + client->getNick() + ' ' + _name + '\n';
        send(client->getFd(), msg.c_str(), msg.length(), 0);     // NOTOPICSET
    }
    else {
        msg = "332 " + client->getNick() + ' ' + _name + ' ' + _topic + '\n';
        send(client->getFd(), msg.c_str(), msg.length(), 0);     // TOPICSET
    }
}

/**
 * @brief           Sets channel topic message
 * @param client    Client that sends the topic message
 * @param topic     Topic message
 */
void Channel::cmdTopic(const std::string &topic, Client *client) {
    std::map<char, bool>::iterator it = _flags.find('t');
    if (finder(_chops, client) && it->second == true) {
        setTopic(topic);
        sendTopic(client);
    }
    else
        ERR_CHANOPRIVSNEEDED(getName(), client->getFd(), _errMsg);
}

/**
 * @brief           Kicks user
 * @param kicked    User to be kicked
 * @param kicker    Chop that kicks
 */
void Channel::cmdKick(Client *kicked, Client *kicker) {
    std::string msgSend;

    if (finder(_users, kicker))
        ERR_NOTONCHANNEL(getName(), kicked->getFd(), _errMsg);
    if (!finder(_chops, kicker))
        ERR_CHANOPRIVSNEEDED(getName(), kicked->getFd(), _errMsg);
    if (finder(_users, kicked)) {
        msgSend =":" + kicker->getNick() + " KICK " + _name + ' ' + kicked->getNick() + '\n';
        kicked->rmvChannel(_name);
        sendMsgToUsers(msgSend);
        rmvClient(kicked);
    }
    else
        ERR_NOSUCHNICK(getName(), kicked->getFd(), _errMsg);
}

void        Channel::cmdInvite(Client *inviter, Client *invited) {
    std::string msgSend;
    
    if (!finder(_chops, inviter))
		ERR_CHANOPRIVSNEEDED(_name, inviter->getFd(), _errMsg);
    msgSend =":" + inviter->getNick() + " INVITE " + invited->getNick() + ' ' + getName() + '\n';
    send(invited->getFd(), msgSend.c_str(), msgSend.length(), 0);
    addInvited(invited);
}



/**
 * @brief           Leave the Channel (using PART Command)
 * @param client    A Pointer to the client that wants to leave the Channel
**/
void Channel::partChannel(Client *client) {                 // Channel ends when there are no more users
    std::string msgSend;

    if (finder(_users, client) || finder(_chops, client)) {
        msgSend = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getIp() + " PART :" + _name + "\r\n";
        sendMsgToUsers(msgSend);
        client->rmvChannel(_name);
        rmvClient(client);
        return ;
    }
    ERR_NOSUCHCHANNEL(_name, client->getFd(), _errMsg);
}

bool Channel::retStateFlag(const char &flag, Client *client) {
    if (finder(_invited_users, client)) {
        return false;
    }
    std::map<char, bool>::iterator it = _flags.find(flag);
    return (it->second);
}

bool Channel::retStateFlag(const char &flag) {
    std::map<char, bool>::iterator it = _flags.find(flag);
    return (it->second);
}
