
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

        //operators
        std::string _topic;
        std::map<char, bool> _flags;


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
        void        initFlags();
        // void        cmdKick(std::string const &topic, Client *client);
        // void        cmdMode(std::string const &topic, Client *client);
        // void        cmdInvite(std::string const &topic, Client *client);
        void        cmdTopic(std::string const &topic, Client *client);
};

#endif
