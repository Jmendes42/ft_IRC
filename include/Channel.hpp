
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
        std::vector<Client *>           _sec_chops;
        std::vector<Client *>           _ban_users;
        std::vector<Client *>           _muted_users;

        //operators
        std::string _topic;
        std::map<char, bool> _flags;
        std::string _password;
        int _user_limit;

    public:

        Channel() { initFlags(); };
        Channel(std::string const &name, Client *chop);
        ~Channel() {};

        void setName(std::string const &set) { _name = set; };
        // void setCHOP(Client *set) { _chop = set; };
        void setTopic(std::string const &set) { _topic = set; };

        std::string             &getName() { return (_name); };
        std::string             &getTopic() { return (_topic); };
        std::string             &getPass() { return (_password); };
        std::vector<Client *>   &getChop() { return (_sec_chops); };

        void    addUser(Client *);
        void    addChop(Client *);
        void    addMute(Client *);
        void    rmvClient(std::string const &nickname);

        void    initFlags();
        void    cmdKick(Client *, const std::string &, int fd);
        void    cmdMode(std::string const &flags, std::string const &args, Client *client);
        void    cmdInvite(Client *client, Client *toInv);
        void    sendTopic(Client *);
        void    cmdTopic(const std::string &, Client *);

        std::string getUsersString();

        std::vector<Client *>   &getUsers() {return _users;};
        std::vector<Client *>   &getChops() {return _sec_chops;};
        std::vector<Client *>   &getMuted() {return _muted_users;};
        int                     getLimit()  {return _user_limit;};
        int                     getUsersTotal()  {return (_users.size() + _sec_chops.size());};



        void changeSimpleFlag(int fd, char set, char flag, std::string const &channel_name);
        void changeModePS(int fd, char set, char flag, std::string const &channel_name);
        void changePassword(int fd, char set, std::string const &args);
        void setLimit(int fd, char set, std::string const &args);

        void    sendMsgToUsers(const std::string &);
        void    sendMsgToUsers(const std::string &, const int &);
        
        Client  *finder(std::vector<Client *> &, const std::string &);

        void partChannel(Client *client);
        bool retStateFlag(char flag);
        bool checkBan(std::string const &nick);

};

#endif
