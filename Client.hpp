

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>


class Client
{
    private:

        std::string _nickname;
        std::string _username;
        std::string _hostname;
        std::string _realname;
        std::string _password;
        //std::vector<Channels *> channels;

    public:

        Client();
        Client(std::string const &nick, std::string const &user, std::string const &host, std::string const &real, std::string const &pass) : 
            _nickname(nick), _username(user), _hostname(host), _realname(real), _password(pass) {} ;
        ~Client() {};

        std::string &getNick() { return (_nickname); };
        std::string &getUser() { return (_username); };
        std::string &getHost() { return (_hostname); };
        std::string &getReal() { return (_realname); };
        std::string &getPass() { return (_password); };

};

#endif