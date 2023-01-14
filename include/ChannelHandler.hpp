#ifndef CHANNEL_HANDLER_HPP
# define CHANNEL_HANDLER_HPP

# include <vector>
# include <string>
# include <iostream>

#define RPL(x) "403 " +  (x) + " :No such channel\r\n"

class Client;
class Channel;

class ChannelHandler
{
    std::vector<Channel *>::iterator    _it;
	std::string                         _errMsg;
    std::vector<Channel *>              _channels;
    public:

        ChannelHandler() {};
        ~ChannelHandler() {};

        void    rmvChannel(std::string const &);
        void    addChannel(std::string const &msg, Client *chop);

        //operators
        Channel *finder(const std::string &);
        //void opMode(const std::vector<std::string> &, Client *);

        std::vector<Channel *>  &getChannels()  {return _channels;};

};

#endif