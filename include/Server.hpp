#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include <vector>
# include <string>
# include <iostream>
# include <stdint.h>

#include "Socket.hpp"
#include "ClientHandler.hpp"
#include "ChannelHandler.hpp"

class Server
{
    int                     _port;
    Socket                  _sock;
	std::string             _errMsg;
    std::string             _password;
	ClientHandler			_clientHandler;
    ChannelHandler          _channelHandler;

    public:
        Server() {};
        Server(const int &port, const std::string &password):
            _port(port), _sock(_port), _password(password) {};

        void    sockSet();
		void	activity();
        void    privMsg(const std::string &, Client *);
        void	partCmd(const std::string &, Client *);
        void    setClientNick(const std::string &, Client *);
        void    setClientUser(const std::string &, Client *);
        void    interpreter(const std::string &, int const &);
        void    opMode(const std::vector<std::string> &, Client *);
        void    joinChannel(const std::vector<std::string> &, Client *);
        void	inviteToChannel(const std::vector<std::string> &, Client *);
        void	quitCmd(Client *);
        
        void    opKick(const std::vector<std::string> &, Client *);

        Socket  &getSocket()    {return _sock;};

};

#endif
