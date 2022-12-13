#ifndef MACROS_HPP
# define MACROS_HPP

#define ERR_NOSUCHCHANNEL(channel, fd, toSend)  toSend =  "403 " +  channel + " :No such channel\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;
#define ERR_NEEDMOREPARAMS(command, fd, toSend) toSend =  "461 " +  command + " :Not enough parameters\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;
#define ERR_INVITEONLYCHAN(channel, fd, toSend) toSend =  "473 " +  channel + " :Cannot join channel (+i)\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;
#define ERR_CHANNELISFULL(channel, fd, toSend) toSend =  "471 " +  channel + " :Cannot join channel (+l)\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;
#define ERR_BANNEDFROMCHAN(channel, fd, toSend) toSend =  "474 " +  channel + " :Cannot join channel (+b)\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;
#define ERR_BADCHANNELKEY(channel, fd, toSend) toSend =  "475" +  channel + " :Cannot join channel (+k)\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;

#define ERR_KEYSET(channel, fd, toSend) toSend =  "467 " + channel + " :Channel key already set\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;

#define ERR_NOTONCHANNEL(channel, fd, toSend) toSend =  "442 " + channel + " :You're not on that channel\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;

#define ERR_CHANOPRIVSNEEDED(channel, fd, toSend) toSend =  "482 " + getName() + " :You're not channel operator\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;

#define ERR_UNKNOWNMODE(channel, fd, toSend) toSend =  "472 " + getName() + " :is unknown mode char to me\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;

#define ERR_NOSUCHNICK(channel, fd, toSend) toSend =  "401 " + getName() + " :No such nick/channel\r\n"; send(fd, toSend.c_str(), toSend.length(), 0); return ;



#endif