#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/Socket.hpp"



void sighandler(int signum) {
    MSG("Turning off server");
    exit(1);
}

// template<typename T>
// T   findert(std::vector<T> &vec, std::string search) {
//     typename std::vector<T>::iterator it;

//     for (it = vec.begin(); it != vec.end(); it++) {
//         if (!(*it)->getNick().compare(search))
//             return (*it);
//     }
//     return NULL;
// }

/*template<typename T>
T   finder(std::vector<T> &vec, int search) {
    typename std::vector<T>::iterator it;

    for (it = vec.begin(); it != vec.end(); it++) {
        if (!(*it)->getFd() == search)
            return (*it);
    }
    return NULL;
}*/

void	*ft_memset(void *s, int c, size_t n)
{
	char	*str;

	str = (char *)s;
	while (n > 0)
	{
		str[n - 1] = c;
		n--;
	}
	return (s);
}


std::vector<std::string> ft_split(std::string const &msg, char const &c)
{
	std::vector<std::string> info;

    int pos = 0;
    int start = 0;
    while ((pos = msg.find(c, start)) > -1)
    {
        info.push_back(msg.substr(start, pos - start));
        start = pos + 1;
    }
    pos = msg.length();
    info.push_back(msg.substr(start, pos - start));

	return (info);
}

void rmvFromVector(int fd, std::string const &channel_name, std::string const &nick, std::vector<Client *> &vec)
{
    if (nick.empty())
    {
        std::string toSend =  "461 MODE :Not enough parameters\r\n";
        send(fd, toSend.c_str(), toSend.length(), 0);
        return ;
    }
	std::vector<Client *>::iterator it;
    for (it = vec.begin(); it != vec.end(); it++)
    {
        if (!((*it)->getNick().compare(nick)))
        {
            vec.erase(it);
            return ;
        }
    }
    std::string toSend =  "401 " + channel_name + " :No such nick/channel\r\n";
    send(fd, toSend.c_str(), toSend.length(), 0);
    return ;
}

void  addToVector(int fd, std::string const &channel_name, std::string const &nickname, std::vector<Client *> &vec, std::vector<Client *> &users)
{
    if (nickname.empty())
    {
        std::string toSend =  "461 MODE :Not enough parameters\r\n";
        send(fd, toSend.c_str(), toSend.length(), 0);
        return ;
    }
    std::vector<Client *>::iterator it_chop;
    std::vector<Client *>::iterator it_clients;

    for (it_chop = vec.begin(); it_chop != vec.end(); it_chop++)
    {

        if (!((*it_chop)->getNick().compare(nickname)))
        {
            std::string toSend =  "467 " + channel_name + " :Channel key already set\r\n";
            send(fd, toSend.c_str(), toSend.length(), 0);
            return ;
        }
    }
    for (it_clients = users.begin(); it_clients != users.end(); it_clients++)
    {
        if (!((*it_clients)->getNick().compare(nickname)))
        {
            std::cout << "Added to Vector" << std::endl;
            vec.push_back((*it_clients));
            return ;
        }
    }
    std::string toSend =  "401 " + channel_name + " :No such nick/channel\r\n";
    send(fd, toSend.c_str(), toSend.length(), 0);
    return ;
}
