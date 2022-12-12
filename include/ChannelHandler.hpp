#ifndef CHANNEL_HANDLER_HPP
# define CHANNEL_HANDLER_HPP

# include <vector>
# include <string>
# include <iostream>

class Client;
class Channel;

class ChannelHandler
{
    std::vector<Channel *>::iterator    _it;
    std::vector<Channel *>              _channels;
    public:

        ChannelHandler() {};
        ~ChannelHandler() {};

        void    rmvClient(std::string const &msg);
        void    addChannel(std::string const &msg, Client *chop);

        //operators
        Channel *finder(const std::string &);
        void opTopic(const std::string &, Client *);
        void opMode(std::string const &msg, Client *chop);

        std::vector<Channel *>  &getChannels()  {return _channels;};

};

#endif