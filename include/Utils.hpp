
#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <iostream>

void	*ft_memset(void *, int, size_t);
std::vector<std::string> ft_split(std::string const &msg, char set);
template<typename T>
void	MSG(T msg) { std::cout << "Message: " << msg << std::endl; };

#endif