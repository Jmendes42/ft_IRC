
#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <iostream>

class Client;

// template<typename T>
// T   findert(std::vector<T> &vec, std::string search);

/*template<typename T>
T   finder(std::vector<T> vec, int search);	*/


void    sighandler(int);
void	*ft_memset(void *, int, size_t);
template<typename T>
void	MSG(T msg) { std::cout << "Message: " << msg << std::endl; };
std::vector<std::string> ft_split(std::string const &msg, char const &c);

void rmvFromVector(int fd, std::string const &channel_name, std::string const &nick, std::vector<Client *> &vec);
void  addToVector(int fd, std::string const &channel_name, std::string const &nickname, std::vector<Client *> &vec, std::vector<Client *> &users);

#endif