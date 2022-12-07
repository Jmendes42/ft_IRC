#ifndef CHANNEL_HANDLER_HPP
# define CHANNEL_HANDLER_HPP

# include "Channel.hpp"

class ChannelHandler
{
    std::vector<Channel *> _channels;
    std::vector<Channel *>::iterator _it;
    public:

        ChannelHandler() {};
        ~ChannelHandler() {};

        void    rmvClient(std::string const &msg);
        void    addChannel(std::string const &msg, Client *chop);

        Channel *finder(const std::string &);
};

#endif