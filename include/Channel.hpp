
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <map>
# include <vector>
# include <string>
# include <iostream>

# include <sys/types.h>
# include <sys/socket.h>

# include "Utils.hpp"
# include "Client.hpp"
// VERIFY: What happens if the CHOP leaves the channel??

class Channel
{
    private:

        std::vector<Client *>::iterator _it;
        std::string                     _name;
        std::vector<Client *>           _users;
        std::vector<Client *>           _chops;
        std::string                     _errMsg;
        std::vector<Client *>           _ban_users;
        std::vector<Client *>           _moderators;
        std::vector<Client *>           _muted_users;
        std::vector<Client *>           _invited_users;

        //operators
        std::string         _topic;
        std::map<char, bool> _flags;
        std::string         _password;
        int                 _user_limit;

    public:

        Channel() { initFlags(); };
        Channel(std::string const &name, Client *chop);
        ~Channel() {};

        void setName(std::string const &set) { _name = set; };
        void setTopic(std::string const &set) { _topic = set; };

        std::string             &getName() { return (_name); };
        std::vector<Client *>   &getChop() { return (_chops); };
        std::string             &getTopic() { return (_topic); };
        std::string             &getPass() { return (_password); };

        void    rmvClient(Client *);
        void    rmvClient(Client *, std::vector<Client *> &);
        void    addInvited(Client *);
        void    addClient(std::vector<Client *> &, Client *, const bool &);

        void    initFlags();
        void    cmdKick(Client *, Client *, const std::string &);
        void    cmdMode(std::string const &flags, std::string const &args, Client *client);
        void    cmdInvite(Client *client, Client *toInv);
        void    sendTopic(Client *);
        void    banUser(const std::string &, Client *, Client *);
        void    cmdTopic(const std::string &, Client *);
        void    chopMode(const std::string &, Client *, Client *);
        void    userMode(const std::string &, Client *, Client *);
        void    moderatorMode(const std::string &, Client *, Client *);

        std::string getUsersString();

        std::vector<Client *>   &getUsers() {return _users;};
        std::vector<Client *>   &getBan() {return _ban_users;};
        std::vector<Client *>   &getChops() {return _chops;};
        int                     getLimit()  {return _user_limit;};
        std::vector<Client *>   &getMuted() {return _muted_users;};
        std::vector<Client *>   &getModerator() {return _moderators;};
        std::vector<Client *>   &getInvited() {return _invited_users;};
        int                     getUsersTotal()  {return (_users.size() + _chops.size());};



        void    changeSimpleFlag(const std::string &, Client *);
        void    changeModePS(Client *client, char set, char flag, std::string const &channel_name);
        void    changePassword(int fd, char set, std::string const &args);
        void    setLimit(int fd, char set, std::string const &args);
        void    changeComposedFlag(const std::string &, const std::string &, Client *);

        void    sendMsgToUsers(const std::string &);
        void    sendMsgToUsers(const std::string &, const int &);
        
        Client  *finder(std::vector<Client *> &, Client *);
        //Client  *finder(std::vector<Client *> &, const std::string &);
        bool    usersOnChannel();

        void    partChannel(Client *);
        bool    retStateFlag(const char &);
        //bool    checkBan(std::string const &);
        bool    usersOnChannel(Client *);
        bool    retStateFlag(const char &, Client *, std::vector<Client *> &);

};

#endif
