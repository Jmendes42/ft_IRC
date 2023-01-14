#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <iostream>
# include <stdint.h>

class Channel;

class Client
{
    private:

        int                                 _fd;
        std::string                         _ip;
        std::vector<Channel *>::iterator    _it;
        bool                                _pass;
        bool                                _registered;
        uint16_t                            _port;
        std::string                         _nickname;
        std::string                         _username;
        std::string                         _hostname;
        std::string                         _realname;
        std::vector<Channel *>              _channels;
        std::string                         _cmdBuffer;

    public:

        Client() {};
       ~Client() {};
        Client(const int &fd, const std::string &ip, const uint16_t &port):_fd(fd), _ip(ip), _pass(false), _registered(false), _port(port) {};

        int                     &getFd()        { return (_fd); };
        std::string             &getIp()        { return (_ip); };
        uint16_t                &getPort()      { return (_port); };
        std::string             &getNick()      { return (_nickname); };
        std::string             &getUser()      { return (_username); };
        std::string             &getHost()      { return (_hostname); };
        std::string             &getReal()      { return (_realname); };
        bool                    getPass()       { return (_pass); };
        bool                    getRegistration() { return (_registered); };
        std::vector<Channel *>  &getChannels()  { return (_channels); };
        std::string             &getCmdBuffer() { return (_cmdBuffer); };


        void    setFd(const int &set)                   { _fd = set; };
        void    setPass()                               { _pass = true; };
        void    setRegistration()                       { _registered = true; };
        void    setNick(std::string const &set)         { _nickname = set; };
        void    setUser(std::string const &set)         { _username = set; };
        void    setHost(std::string const &set)         { _hostname = set; };
        void    setReal(std::string const &set)         { _realname = set; };
        void    setCmdBuffer(std::string const &set)    { _cmdBuffer = set; };

        void    addChannel(Channel *);
        void    rmvChannel(const std::string &);
        Channel *findChannel(const std::string &);

};

#endif