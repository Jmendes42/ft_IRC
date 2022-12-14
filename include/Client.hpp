#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <iostream>

class Channel;

class Client
{
    private:

        std::vector<Channel *>::iterator    _it;
        std::string                         _ip;
        int                                 _fd;
        int                                 _pass;
        uint16_t                            _port;
        std::string                         _nickname;
        std::string                         _username;
        std::string                         _hostname;
        std::string                         _realname;
        std::vector<Channel *>              _channels;

    public:

        Client() {};
        ~Client() {};
        Client(const int &fd, const std::string &ip, const uint16_t &port):_fd(fd), _ip(ip), _pass(1), _port(port) {};

        int                     &getFd()        { return (_fd); };
        std::string             &getIp()        { return (_ip); };
        uint16_t                &getPort()      { return (_port); };
        std::string             &getNick()      { return (_nickname); };
        std::string             &getUser()      { return (_username); };
        std::string             &getHost()      { return (_hostname); };
        std::string             &getReal()      { return (_realname); };
        std::vector<Channel *>  &getChannels()  { return (_channels); };

        void    setFd(const int &set)           { _fd = set; };
        void    setNick(std::string const &set) { _nickname = set; };
        void    setUser(std::string const &set) { _username = set; };
        void    setHost(std::string const &set) { _hostname = set; };
        void    setReal(std::string const &set) { _realname = set; };

        void    addChannel(Channel *);
        void    rmvChannel(const std::string &);
        Channel *findChannel(const std::string &);
};

#endif