
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ClientHandler.hpp"

// VERIFY: What happens if the CHOP leaves the channel??

class Channel
{
    private:

        std::string _name;
        Client      *_chop;
        std::vector<Client *> _users;
        std::vector<Client *> _sec_chops;


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
        void setCHOP(Client *set) { _chop = set; };
        void setTopic(std::string const &set) { _topic = set; };


        std::string &getName() { return (_name); };
        std::string &getChop() { return (_chop->getNick()); };
        std::string &getTopic() { return (_topic); };


        void        addUser(Client *user);
        void        rmvUser(std::string const &nickname);



        //operators
        void        addChopp(std::string const &nickname);
        void        rmvChop(std::string const &nickname);

        void        initFlags();
        // void        cmdKick(std::string const &topic, Client *client);
        void        cmdMode(std::string const &flags, std::string const &args, Client *client);
        // void        cmdInvite(std::string const &topic, Client *client);
        void        cmdTopic(std::string const &topic, Client *client);

        void changeSimpleFlag(char set, char flag);
        void changeModePS(char set, char flag);
        void changePassword(char set, std::string const &args);
};

#endif
