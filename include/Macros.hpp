#ifndef MACROS_HPP
# define MACROS_HPP

#define ERR_NICKNAMEINUSE(client, fd, errMsg)           {errMsg = "433 " + client + " :Nikname in use\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOSUCHNICK(client, fd, errMsg)              {errMsg = "401 " + client + " :No such nickname\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOSUCHCHANNEL(channel, fd, errMsg)          {errMsg = "403 " + channel + " :No such channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_USERNOTINCHANNEL(channel, nick, fd, errMsg) {errMsg = "441 :" + nick + " not in channel " + channel + "\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}  // NEEDS TESTIN
#define ERR_NOTONCHANNEL(channel, fd, errMsg)           {errMsg = "442 " + channel + " :You're not on that channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_USERONCHANNEL(channel, fd, errMsg)          {errMsg = "443 " + channel + " :Already on channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NEEDMOREPARAMS(command, fd, errMsg)         {errMsg = "461 " + command + " :Not enough parameters\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_KEYSET(channel, fd, errMsg)                 {errMsg = "467 " + channel + " :Channel key already set\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_CHANNELISFULL(channel, fd, errMsg)          {errMsg = "471 " + channel + " :Cannot join channel (+l)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_UNKNOWNMODE(channel, fd, errMsg)            {errMsg = "472 " + channel + " :is unknown mode char to me\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_INVITEONLYCHAN(channel, fd, errMsg)         {errMsg = "473 " + channel + " :Cannot join channel (+i)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_BANNEDFROMCHAN(channel, fd, errMsg)         {errMsg = "474 " + channel + " :Cannot join channel (+b)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_BADCHANNELKEY(channel, fd, errMsg)          {errMsg = "475 " + channel + " :Cannot join channel (+k)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_CHANOPRIVSNEEDED(channel, fd, errMsg)       {errMsg = "482 " + channel + " :You're not channel operator\\moderator\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}

#endif