#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include <vector>
# include <string>
# include <iostream>

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
        void    privMsg(const std::string &, const int &);
        void    interpreter(const std::string &, int const &);
        void    joinChannel(const std::vector<std::string> &, const int &);
        void    setClientNick(const std::string &, Client *);
        void    setClientUser(const std::string &, Client *);
        void	inviteToChannel(const std::vector<std::string> &, const int &);
        void	partCmd(const std::string &, const int &);
        void	quitCmd(Client *);
        
        void    opKick(const std::vector<std::string> &, const std::string &, const int &);

        Socket  &getSocket()    {return _sock;};

};

#endif
