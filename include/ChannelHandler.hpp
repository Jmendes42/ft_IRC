#ifndef CHANNEL_HANDLER_HPP
# define CHANNEL_HANDLER_HPP

# include "Channel.hpp"

class ChannelHandler
{
    private:

        std::vector<Channel *> _channels;

    public:

        ChannelHandler() {};
        ~ChannelHandler() {};

        void    rmvClient(std::string const &msg);
        void    addChannel(std::string const &msg, Client *chop);

        //operators
        void    opCommands(std::string &msg, Client *chop, chopCommand cmd);
        Channel *ChannelHandler::findChannel(std::string const &name);
};

#endif