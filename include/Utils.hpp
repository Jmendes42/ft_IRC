
#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <iostream>


// template<typename T>
// T   findert(std::vector<T> &vec, std::string search);

/*template<typename T>
T   finder(std::vector<T> vec, int search);	*/


void	*ft_memset(void *, int, size_t);
template<typename T>
void	MSG(T msg) { std::cout << "Message: " << msg << std::endl; };

std::vector<std::string> ft_split(std::string const &msg);
void rmvFromVector(std::string const &nick, std::vector<Client *> &vec);
void  addToVector(std::string const &nickname, std::vector<Client *> &vec, std::vector<Client *> &users);


#endif