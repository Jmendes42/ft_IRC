#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include "ClientHandler.hpp"
# include "Socket.hpp"

class Server
{
    int                     _port;
    Socket                  _sock;
    std::string             _password;
	ClientHandler			_handler;

    public:
        Server() {};
        Server(const int &port, const std::string &password):
            _port(port), _password(password), _sock(_port) {};

        void    _chat();
        void    _sockSet();
        void    _interpreter(std::string const &);
};

#endif
