#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include "Utils.hpp"
# include "Socket.hpp"
# include "ClientHandler.hpp"
# include "ChannelHandler.hpp"

class Server
{
    int                     _port;
    Socket                  _sock;
    std::string             _password;
	ClientHandler			_clientHandler;
    ChannelHandler          _channelHandler;

    public:
        Server() {};
        Server(const int &port, const std::string &password):
            _port(port), _sock(_port), _password(password) {};

        void    _sockSet();
		void	activity();
        void    privMsg(const std::string &, const int &);
        void    interpreter(std::string const &, int const &);
        void    joinChannel(const std::string &, const int &);
        void    setClientNick(const std::string &, const int &);
        void    setClientUser(const std::string &, const int &);
        Socket  &getSocket()    {return _sock;};

};

#endif
