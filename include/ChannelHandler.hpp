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
    std::vector<Channel *> _channels;
    std::vector<Channel *>::iterator _it;
    public:

        ChannelHandler() {};
        ~ChannelHandler() {};

        void    rmvClient(std::string const &msg);
        void    addChannel(std::string const &msg, Client *chop);

        //operators
        Channel *finder(const std::string &);
        void opTopic(const std::string &, Client *);
        void opMode(std::string const &msg, Client *chop);
        void opKick(const std::string &, const std::string &, int fd);

        std::vector<Channel *>  &getChannels()  {return _channels;};

};

#endif