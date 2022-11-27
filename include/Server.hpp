#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include "ClientHandler.hpp"

class Server
{
    int                     _port;
    Socket                  _sock;
    std::string             _password;
	ClientHandler			_clientHandler;
    // ChannelHandler          _channelHandler;

    public:
        Server() {};
        Server(const int &port, const std::string &password):
            _port(port), _sock(_port), _password(password) {};

        void    _chat();
        void    _sockSet();
        void    _interpreter(std::string const &, int const &);
};

#endif
