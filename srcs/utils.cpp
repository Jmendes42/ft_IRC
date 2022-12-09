#include "../include/Utils.hpp"



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


std::vector<std::string> ft_split(std::string const &msg)
{
	std::vector<std::string> info;
    int pos = 0;
    int start = 0;
    while ((pos = msg.find(" ", start)) > -1)

    {
        info.push_back(msg.substr(start, pos - start));
        start = pos + 1;
    }
    pos = msg.length();
    info.push_back(msg.substr(start, pos - start));

	return (info);
}

void rmvFromVector(std::string const &nick, std::vector<Client *> &vec)
{
	std::vector<Client *>::iterator it;
    for (it = vec.begin(); it != vec.end(); it++)
    {
        if (!((*it)->getNick().compare(nick)))
        {
            vec.erase(it);
            return ;
        }
    }
    std::cout << "ERROR: User not Found in the Vector" << std::endl;
}

void  addToVector(std::string const &nickname, std::vector<Client *> &vec, std::vector<Client *> &users)
{
    if (nickname.empty())
    {
        std::cout << "ERROR: NO NICKNAME" << std::endl;
        return ;
    }
    std::vector<Client *>::iterator it_chop;
    std::vector<Client *>::iterator it_clients;

    for (it_chop = vec.begin(); it_chop != vec.end(); it_chop++)
    {

        if (!((*it_chop)->getNick().compare(nickname)))
        {
            std::cout << "ERROR: This User is already in the Vect" << std::endl;
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
    std::cout << "ERROR: This User is not in the Channel" << std::endl;
}
