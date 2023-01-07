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
    std::vector<Client *>::iterator _it;
    int                             _port;
    Socket                          _sock;
    std::vector<Client *>           _opers;
	std::string                     _errMsg;
    std::string                     _password;
    std::string                     _opUserName;
    std::string                     _opPassword;
	ClientHandler			        _clientHandler;
    ChannelHandler                  _channelHandler;

    public:
        Server() {};
        Server(const int &port, const std::string &password):
            _port(port), _sock(_port), _password(password) 
                {_opUserName = "H"; _opPassword = "4";}

        void    new_connection();
        void	io_operations(char *buffer, int i);
        void    sockSet();
	    void	activity();

        void    interpreter(const std::string &, int const &);
        void    setClientNick(std::vector<std::string>, Client *);
        void    setClientUser(std::vector<std::string>, Client *);
        void    opMode(const std::vector<std::string> &, Client *);
        void	partCmd(const std::vector<std::string> &, Client *);
        void	killCmd(const std::vector<std::string> &, Client *);

        void	pong(Client *);
        void	quitCmd(Client *);
        
        void    opKick(const std::vector<std::string> &, Client *);
        void    message(const std::vector<std::string> &, Client *);
        void	inviteToChannel(std::vector<std::string>, Client *);
        void    operCmd(const std::vector<std::string> &, Client *);
        void    joinChannel(const std::vector<std::string> &, Client *);

        void	addOper(Client *);
        void	delOper(Client *);
        Client *operFinder(Client *);
        void	destroyClient(Client *);

        Socket  &getSocket()    {return _sock;};
};

#endif
