#include "../include/Utils.hpp"

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

