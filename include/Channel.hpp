
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

    public:

        Channel() {};
        Channel(std::string const &name, Client *chop);
        ~Channel() {};

        void setName(std::string const &set) { _name = set; };
        void setCHOP(Client *set) { _chop = set; };

        std::string &getName() { return (_name); };
        std::string &getChop() { return (_chop->getNick()); };

        void        addUser(Client *user);
        void        rmvUser(std::string const &nickname);

};

#endif
