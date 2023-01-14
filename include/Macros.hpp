#ifndef MACROS_HPP
# define MACROS_HPP

#define SEND(fd, sendMsg)                               {send(fd, sendMsg.c_str(), sendMsg.length(), 0);}
#define ERASE_VEC(vec, _it)                             {vec.erase(_it); return;}
#define ERASE_DEL_VEC(vec, _it)                         {delete (*_it); vec.erase(_it); return;}

#define ERR_NEEDMOREPARAMS_CONT(command, fd, errMsg)    {errMsg = "461 " + command + " :Not enough parameters\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue; ;}
#define RPL_YOUREOPER(fd, errMsg)                       {errMsg = "381 :You are now an IRC operator\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOSUCHNICK(client, fd, errMsg)              {errMsg = "401 " + client + " :No such nickname\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOSUCHNICK_CONT(client, fd, errMsg)         {errMsg = "401 " + client + " :No such nickname\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue ;}
#define ERR_NOSUCHCHANNEL_CONT(channel, fd, errMsg)     {errMsg = "403 " + channel + " :No such channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue; ;}
#define ERR_NOSUCHCHANNEL(channel, fd, errMsg)          {errMsg = "403 " + channel + " :No such channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOSUCHCHANNEL_CONT(channel, fd, errMsg)     {errMsg = "403 " + channel + " :No such channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue; ;}
#define ERR_CANNOTSENDTOCHAN(channel, fd, errMsg)       {errMsg = "404 " + channel + " :Cannot send to channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NORECIPIENT(fd, errMsg)                     {errMsg = "411 :No recipient given (PRIVMSG)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOTEXTTOSEND(channel, fd, errMsg)           {errMsg = "412 " + channel + " :No text to send\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NONICKNAMEGIVEN(client, fd, errMsg)         {errMsg = "431 " + client + " :No nickname given\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_ERRONEUSNICKNAME(client, nick, fd, errMsg)  {errMsg = "432 " + client + ' ' + nick + " :Erroneus nickname\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NICKNAMEINUSE(client, fd, errMsg)           {errMsg = "433 " + client + " :Nikname in use\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NICKCOLLISION(client, fd, errMsg)           {errMsg = "436 " + client + " :Nickname collision KILL\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_USERNOTINCHANNEL(nick, channel, fd, errMsg) {errMsg = "441 :" + nick + " not in channel " + channel + "\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}  
#define ERR_NOTONCHANNEL(channel, fd, errMsg)           {errMsg = "442 " + channel + " :You're not on that channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_USERONCHANNEL(channel, fd, errMsg)          {errMsg = "443 " + channel + " :Already on channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_USERONCHANNEL_CONT(channel, fd, errMsg)     {errMsg = "443 " + channel + " :Already on channel\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue; ;}
#define ERR_NEEDMOREPARAMS(command, fd, errMsg)         {errMsg = "461 " + command + " :Not enough parameters\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NEEDMOREPARAMS_CONT(command, fd, errMsg)    {errMsg = "461 " + command + " :Not enough parameters\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue; ;}
#define ERR_PASSWDMISMATCH(client, fd, errMsg)          {errMsg = "464 " + client + " :Incorrect Password\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_KEYSET(channel, fd, errMsg)                 {errMsg = "467 " + channel + " :Channel key already set\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_CHANNELISFULL(channel, fd, errMsg)          {errMsg = "471 " + channel + " :Cannot join channel (+l)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_CHANNELISFULL_CONT(channel, fd, errMsg)     {errMsg = "471 " + channel + " :Cannot join channel (+l)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue ;}
#define ERR_UNKNOWNMODE(channel, fd, errMsg)            {errMsg = "472 " + channel + " :is unknown mode char to me\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_INVITEONLYCHAN(channel, fd, errMsg)         {errMsg = "473 " + channel + " :Cannot join channel (+i)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_INVITEONLYCHAN_CONT(channel, fd, errMsg)    {errMsg = "473 " + channel + " :Cannot join channel (+i)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue ;}
#define ERR_BANNEDFROMCHAN(channel, fd, errMsg)         {errMsg = "474 " + channel + " :Cannot interact with channel (+b)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_BANNEDFROMCHAN_CONT(channel, fd, errMsg)    {errMsg = "474 " + channel + " :Cannot interact with channel (+b)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue ;}
#define ERR_BADCHANNELKEY(channel, fd, errMsg)          {errMsg = "475 " + channel + " :Cannot join channel (+k)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_BADCHANNELKEY_CONT(channel, fd, errMsg)     {errMsg = "475 " + channel + " :Cannot join channel (+k)\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); continue; ;}
#define ERR_NOPRIVILEGES(client, fd, errMsg)            {errMsg = "481 " + client + " :Permission Denied - You're not an IRC operator\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_CHANOPRIVSNEEDED(channel, fd, errMsg)       {errMsg = "482 " + channel + " :You're not channel operator\\moderator\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}
#define ERR_NOTREGISTERED(cmd, fd, errMsg)              {errMsg = "451 " + cmd + " :Not Properly Registered\r\n"; send(fd, errMsg.c_str(), errMsg.length(), 0); return ;}

#endif