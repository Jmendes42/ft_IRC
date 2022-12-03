#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Socket.hpp"

class Client
{
    private:

        std::string _ip;
        int         _fd;
        int         _pass;
        int         _port;
        std::string _nickname;
        std::string _username;
        std::string _hostname;
        std::string _realname;
        // std::vector<Channel *> _channels;

    public:

        ~Client() {};
        Client() { _pass = 1;};

        std::string &getNick() { return (_nickname); };
        std::string &getUser() { return (_username); };
        std::string &getHost() { return (_hostname); };
        std::string &getReal() { return (_realname); };

        void setNick(std::string const &set) { _nickname = set; };
        void setUser(std::string const &set) { _username = set; };
        void setHost(std::string const &set) { _hostname = set; };
        void setReal(std::string const &set) { _realname = set; };

        // void addChannel(Channel *add);
        // void rmvChannel(std::string &name);
};

#endif