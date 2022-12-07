
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ClientHandler.hpp"

// VERIFY: What happens if the CHOP leaves the channel??

class Channel
{
    private:

        std::vector<Client *>::iterator _it;
        std::string                     _name;
        Client                          *_chop;
        std::vector<Client *>           _users;

    public:

        Channel() {};
        Channel(std::string const &name, Client *chop);
        ~Channel() {};

        void setCHOP(Client *set)               { _chop = set; };
        void setName(std::string const &set)    { _name = set; };

        std::string &getName() { return (_name); };
        std::string &getChop() { return (_chop->getNick()); };

        void        addUser(Client *user);
        void        rmvUser(std::string const &nickname);

        std::string getUsersString();

        std::vector<Client *>   &getUsers() {return _users;};
};

#endif
