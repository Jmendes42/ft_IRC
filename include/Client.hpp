#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Socket.hpp"

class Client
{
    private:

        int         _pass;
        std::string _nickname;
        std::string _username;
        std::string _hostname;
        std::string _realname;
        std::vector<Channel *> _channels;

    public:

        Client() { _pass = 1;};
        ~Client() {};

        void setNick(std::string const &set) { _nickname = set; };
        void setUser(std::string const &set) { _username = set; };
        void setHost(std::string const &set) { _hostname = set; };
        void setReal(std::string const &set) { _realname = set; };

        std::string &getNick() { return (_nickname); };
        std::string &getUser() { return (_username); };
        std::string &getHost() { return (_hostname); };
        std::string &getReal() { return (_realname); };

        void addChannel(Channel *add);
        void rmvChannel(std::string &name);
};

#endif