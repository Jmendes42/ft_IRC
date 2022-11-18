
#ifndef CLIENT_HANDLER_HPP
# define CLIENT_HANDLER_HPP

# include "header.hpp"


class ClientHandler
{
    private:

        std::vector<Client *> _clients;

    public:

        void    editClient(std::vector<std::string> &info, int control);
        void    addClient(std::string const &msg, std::string const &pass);
        void    rmvClient(std::string const &msg);

};

void    ClientHandler::editClient(std::vector<std::string> &info, int control)
{
    if (control == 0)
        _clients.back()->setNick(info[1]);
    else
    {
        _clients.back()->setUser(info[1]);
        _clients.back()->setHost(info[2]);
        _clients.back()->setReal(info[4]);
        MSG("Client Created");
        std::cout << "Nick-- " << _clients[0]->getNick() << " --User-- " << _clients[0]->getUser()
            << " --Host-- " << _clients[0]->getHost() << " --Real-- " << _clients[0]->getReal() << std::endl;
    }
}

void    ClientHandler::addClient(std::string const &msg, std::string const &pass)
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
    info.push_back(msg.substr(start, pos - start - 2)); //msg.find(' ') - 1)

    MSG(pos);
    MSG(start);

    if (!info[0].compare("PASS")) {
        if (!info[1].compare(pass)) {
            _clients.push_back(new Client());
            MSG(info[1]);
    }
        else {
            MSG(info[1]);
            MSG("Error: Wrong password!");
            exit(0);
        }
    }
    else if (!info[0].compare("NICK")) {
        MSG(info[1]);
        editClient(info, 0);
    }
    else if (!info[0].compare("USER")) {
        MSG(info[1]);
        editClient(info, 1);
    }
}

// REMOVE BY NICK ! CHECK THE COMMAND TO REMOVE USER TO CHECK WHAT IS NEEDED
void    ClientHandler::rmvClient(std::string const &msg)
{
    std::vector<Client *>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
    {
        if (!((*it)->getNick().compare(msg)))
            _clients.erase(it);
    }
}

#endif