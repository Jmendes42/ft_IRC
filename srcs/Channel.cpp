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
 * @brief           returns true if the user is on the channel and false if it's not
 * @param nick      nickname to search for
**/
bool    Channel::usersOnChannel(Client *find) {
    if (finder(_users, find) || finder(_chops, find) || finder(_muted_users, find)
            || finder(_moderators, find))
        return true;
    return false;
}

/**
 * @brief           returns true if there's users on the channel and false it's empty
**/
bool    Channel::usersOnChannel() {
    if (_users.empty() && _chops.empty() && _muted_users.empty())
        return false;
    return true;
}

/**
 * @brief           Get a string with the nicks of all the clients present in the channel with the respective indicator
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
 * @brief           Send message to every client in channel
 * @param msg       msg to send
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
 * @brief           Send message to every client in channel excluding one fd
 * @param fd        fd to exclude
 * @param msg       msg to send
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
 * @brief           Searches all the client groups for the nick and removes it from the channel vectors
 * @param nick      Nick to search 
 */
void    Channel::rmvClient(Client *rmv) {
    if (finder(_chops, rmv))
        ERASE_VEC(_chops, _it);
    if (finder(_users, rmv))
        ERASE_VEC(_users, _it);
    if (finder(_ban_users, rmv))
        ERASE_VEC(_ban_users, _it);
    if (finder(_moderators, rmv))
        ERASE_VEC(_moderators, _it);
    if (finder(_muted_users, rmv))
        ERASE_VEC(_muted_users, _it);
}

void    Channel::rmvClient(Client *rmv, std::vector<Client *> &vec) {
    if (finder(vec, rmv))
        ERASE_VEC(vec, _it);
}

/**
 * @brief           Set the password of the Channel.
 * @param args      The new password for the channel.
 * @param set       Control if the function will set or unset the password
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
    else if (set == '+' && !args.empty())
    {
        _password = args;
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
    if (vec != _ban_users)
        rmvClient(client);
    vec.push_back(client);
}

/**
 * @brief           Checks if the user can be banned, if it can adds it to the banned vector
 * @param user      User to ban 
 * @param flag      Ban flag
 */
void    Channel::banUser(const std::string &flag, Client *user, Client *chop) {
    std::string msgSend;

    if (flag[0] == '+' && finder(_ban_users, user))
        return ;
    else if (flag[0] == '-' && !finder(_ban_users, user))
        return ;
    (flag[0] == '+') ? addClient(_ban_users, user) : rmvClient(user, _ban_users);
    msgSend = ':' + chop->getNick() + " MODE " + _name + ' ' + flag + ' ' + user->getNick() + "\r\n";
    sendMsgToUsers(msgSend);
}

/**
 * @brief           Checks if the user can be promoted to chop, if it can adds it to the chops vector
 * @param user      User to promote 
 * @param flag      Chop promote flag
 */
void    Channel::chopMode(const std::string &flag, Client *user, Client *chop) {
    std::string msgSend;

    if ((flag[0] == '+' && finder(_chops, user)) || (flag[0] == '-' && !finder(_chops, user)))
        ERR_KEYSET(_name, chop->getFd(), _errMsg)
    (flag[0] == '+') ? addClient(_chops, user) : addClient(_users, user);
    msgSend = ':' + chop->getNick() + " MODE " + _name + ' ' + flag + ' ' + user->getNick() + "\r\n";
    sendMsgToUsers(msgSend);
}

/**
 * @brief           Checks if the user can be promoted to moderator, if it can adds it to the moderators vector
 * @param user      User to promote 
 * @param flag      Moderator promote flag
 */
void    Channel::moderatorMode(const std::string &flag, Client *user, Client *chop) {
    std::string msgSend;

    if (!finder(_chops, chop))
        ERR_CHANOPRIVSNEEDED(_name, chop->getFd(), _errMsg)
    if ((flag[0] == '+' && finder(_moderators, user)) || (flag[0] == '-' && !finder(_moderators, user)))
        ERR_KEYSET(_name, chop->getFd(), _errMsg)
        
    (flag[0] == '+') ? addClient(_moderators, user) : addClient(_users, user);

    // THIS IS NOT A GOOD SOLUTION. rmvClient() uses ERASE_VEC that returns, so it never actually erases it from _moderators... Check if other functions are using it and what is the best solution for this
    if (flag[0] == '-')
    {
        if (finder(_moderators, user))
            _moderators.erase(_it);
    }

    msgSend = ':' + chop->getNick() + " MODE " + _name + ' ' + flag + ' ' + user->getNick() + "\r\n";
    sendMsgToUsers(msgSend);
}

/**
 * @brief           /mode command flag redirect
 * @param chop      Issuer of the command 
 * @param user      Target client 
 * @param flags     Flag
 */
void    Channel::userMode(const std::string &flags, Client *user, Client *chop) {
    if (!usersOnChannel(user) && !finder(_ban_users, user))
        ERR_USERNOTINCHANNEL(chop->getNick(), user->getNick(), _name, chop->getFd(), _errMsg)
    if (flags[1] == 'b')
        banUser(flags, user, chop);
    else if (flags[1] == 'o')
        chopMode(flags, user, chop);
    else if (flags[1] == 'v')
        moderatorMode(flags, user, chop);
}

/**
 * @brief   Create the Flags Map
 */
void Channel::initFlags()
{
    _flags.insert(std::pair<char, bool>('o', false));       //  o - give/take channel operator privileges;
    _flags.insert(std::pair<char, bool>('p', false));       //  p - private channel flag;
    _flags.insert(std::pair<char, bool>('s', false));       //  s - secret channel flag;
    _flags.insert(std::pair<char, bool>('i', false));       //  i - invite-only channel flag;
    _flags.insert(std::pair<char, bool>('t', true));       //  t - topic settable by channel operator only flag;
    _flags.insert(std::pair<char, bool>('n', true));        //  n - no messages to channel from clients on the outside;
    _flags.insert(std::pair<char, bool>('m', false));       //  m - moderated channel;
    _flags.insert(std::pair<char, bool>('l', false));       //  l - set the user limit to channel;
    _flags.insert(std::pair<char, bool>('b', false));       //  b - set a ban mask to keep users out;
    _flags.insert(std::pair<char, bool>('v', false));       //  v - give/take the ability to speak on a moderated channel;
    _flags.insert(std::pair<char, bool>('k', false));       //  k - set a channel key (password).
}

/**
 * @brief           Set the User limit of the Channel.
 * @param set       Control if the function will set or unset the limit 
 * @param args      The new password for the channel.
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
 * @brief           Change the state of '-p' or '-s' flag. Notice that both flags can't be 'true' at the same time. If you set one to true, the other will be set to false. More info in README
 * @param set       control if the function will set the flag to true or false (+/-)
 * @param flag      the flag of the channel that will change state
**/
void Channel::changeModePS(Client *client, char set, char flag, std::string const &channel_name)
{
    std::map<char, bool>::iterator it = _flags.find(flag);
    std::map<char, bool>::iterator temp;
    std::string msgSend;
    
    temp = (it->first == 'p') ?  _flags.find('s') : _flags.find('p');
    if (set == '+' && it->second == false)
    {
        it->second = true;
        if (temp->second == true)
            temp->second = false;
        msgSend = ':' + client->getNick() + " MODE " + channel_name + " +" + flag + "\r\n";
        sendMsgToUsers(msgSend);
    }
    else if (set == '-' && it->second == true)
    {
        it->second = false;
        msgSend = ':' + client->getNick() + " MODE " + channel_name + " -" + flag + "\r\n";
        sendMsgToUsers(msgSend);
    }
    else
        ERR_KEYSET(channel_name, client->getFd(), _errMsg);
}

/**
 * @brief           Checks if the flag is Simple, PS or Complex
 * @param flag      Flag to be checked
**/
int checkFlag(char flag)
{
    if (flag == 'p' || flag == 's')
        return (1);
    else if (flag == 'i' || flag == 't' || flag == 'n' || flag == 'm')
        return (2);
    else if (flag == 'l' || flag == 'k')
        return (3);
    return (0);
}

/**
 * @brief           Change the state of "simple flags". What is true, will be false and vice-versa. A warning will be throwed if flag was already set.
 * @param set       Control if the function will set the flag to true or false (+/-)
 * @param flag      the flag of the channel that will change state
**/
void Channel::changeSimpleFlag(const std::string &flag, Client *chop) {
    std::string msgSend;

    std::map<char, bool>::iterator it = _flags.find(flag[1]);
    if (flag[0] == '+' && it->second == false)
        it->second = true;
    else if (flag[0] == '-' && it->second == true)
        it->second = false;
    else
        ERR_KEYSET(_name, chop->getFd(), _errMsg);
    msgSend = ':' + chop->getNick() + " MODE " + _name + ' ' + flag + "\r\n";
    sendMsgToUsers(msgSend);

}

/**
 * @brief       Change the state of "Composed Flags" and set the key/limit of the channel
 * @param flag  the flag of the channel that will change state
**/
void    Channel::changeComposedFlag(const std::string &flag, const std::string &arg, Client *chop) {
    std::string msgSend;

    if (flag[1] == 'l') 
        setLimit(chop->getFd(), flag[0], arg);
    if (flag[1] == 'k') {
        changePassword(chop->getFd(), flag[0], arg);
    }
    msgSend = ':' + chop->getNick() + " MODE " + _name + ' ' + flag + ' ' + arg + "\r\n";
    sendMsgToUsers(msgSend);
}

void Channel::cmdMode(std::string const &flags, std::string const &args, Client *chop) {
    std::string msgSend;

    if (!finder(_users, chop) && !finder(_chops, chop))
        ERR_NOTONCHANNEL(getName(), chop->getFd(), _errMsg);
    char set = flags[0];

    for (long unsigned int i = 1; i < flags.length(); i++) {
        // In case of P or S ! Only one can be set at a time
        if (checkFlag(flags[i]) == 1)
            changeModePS(chop, set, flags[i], getName());
        else if (checkFlag(flags[i]) == 2)                          
            changeSimpleFlag(flags, chop);
        else if (checkFlag(flags[i]) == 3)
            changeComposedFlag(flags, args, chop);
        else
            ERR_UNKNOWNMODE(getName(), chop->getFd(), _errMsg);
        
    }
}

/**
 * @brief           Prints channel topic to client
 * @param client    Client that requested
 */
void Channel::sendTopic(Client *client) {
    std::string sendMsg;
    if (_topic.empty()) {
        sendMsg = "331 " + client->getNick() + ' ' + _name + '\n';
        SEND(client->getFd(), sendMsg)
        return ;
    }
    sendMsg = "332 " + client->getNick() + ' ' + _name + ' ' + _topic + '\n';
    SEND(client->getFd(), sendMsg)
}

/**
 * @brief           Sets channel topic message
 * @param client    Client that sends the topic message
 * @param topic     Topic message
 */
void Channel::cmdTopic(const std::string &topic, Client *client) {
    std::string sendMsg;

    setTopic(topic);
    sendMsg =":" + client->getNick() + " TOPIC " + _name + " " + _topic + "\r\n";
    sendMsgToUsers(sendMsg);
}

/**
 * @brief           Kicks user
 * @param kicked    User to be kicked
 * @param kicker    Chop that kicks
 */
void Channel::cmdKick(Client *kicked, Client *kicker, const std::string &args) {
    std::string msgSend;

    if (!usersOnChannel(kicker))
        ERR_NOTONCHANNEL(getName(), kicker->getFd(), _errMsg);
    if (!usersOnChannel(kicked))
        ERR_NOSUCHNICK(getName(), kicker->getFd(), _errMsg);
    if (!finder(_chops, kicker))
        ERR_CHANOPRIVSNEEDED(getName(), kicker->getFd(), _errMsg);
    msgSend = ':' + kicker->getNick() + ' ' + args + "\r\n";
    kicked->rmvChannel(_name);
    sendMsgToUsers(msgSend);
    rmvClient(kicked);
}

/**
 * @brief           Sends invite message to a user
 * @param invited   User to be invited
 * @param inviter   Chop that invites
 */
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
void Channel::partChannel(Client *client) {
    std::string msgSend;

    //if (finder(_users, client) || finder(_chops, client)) {
    if (usersOnChannel(client)) {
        msgSend = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getIp() + " PART :" + _name + "\r\n";
        sendMsgToUsers(msgSend);
        client->rmvChannel(_name);
        rmvClient(client);
        return ;
    }
    ERR_NOSUCHCHANNEL(_name, client->getFd(), _errMsg);
}

/**
 * @brief         Return the state of a flag in the moment (true or false) - Used for Flags that have a proper vector
 * @param flag    Key to be checked
 * @param client  To find in the Flags vector
 * @param client  Flags vector
**/
bool Channel::retStateFlag(const char &flag, Client *client, std::vector<Client *> &vec) {
    if (finder(vec, client)) {
        return false;
    }
    std::map<char, bool>::iterator it = _flags.find(flag);
    return (it->second);
}

/**
 * @brief         Return the state of a flag in the moment (true or false)
 * @param flag    Key to be checked
**/
bool Channel::retStateFlag(const char &flag) {
    std::map<char, bool>::iterator it = _flags.find(flag);
    return (it->second);
}
