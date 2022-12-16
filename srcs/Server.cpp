# include <vector>
# include <string>
# include <iostream>

#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Socket.hpp"
#include "../include/Macros.hpp"

void	Server::activity()
{
	char		buffer[1024];
	std::string message;

	_sock.setActivity(select(_sock.getMaxSd() + 1 , &_sock.getReadFds() , NULL , NULL , NULL));
	if ((_sock.getActivity() < 0) && (errno != EINTR))
	{
		MSG("Select Error");
		throw ConnectionException();
	}
	// If something happened on the master socket,
	// then its an incoming connection
	if (FD_ISSET(_sock.getSocketFd(), &_sock.getReadFds()))
	{
		_sock.setNewSocket(accept(_sock.getSocketFd(), (struct sockaddr *)&_sock.getHint(), (socklen_t*)&_sock.getClientSize()));
		if (_sock.getNewSocket() < 0)
		{
			MSG("Accepting Error");
			throw ConnectionException();
		}
		// Inform user of socket number - used in send and receive commands
		std::cout << "New connection , socket fd is " << _sock.getNewSocket() 
			<< ", IP is: " << inet_ntoa(_sock.getHint().sin_addr) 
				<< ", port: " << ntohs(_sock.getHint().sin_port) << std::endl;
		// Add new socket to array of sockets
		for (int i = 0; i < _sock.getMaxClients(); i++)
		{
			// If position is empty
			if (_sock.getClientSocket(i) == 0 )
			{
				_sock.setClientSocket(i, _sock.getNewSocket());
				printf("Adding to list of sockets as %d\n" , i);
				break;
			}
		}
	}
	// Else its some IO operation on some other socket
	for (int i = 0; i < _sock.getMaxClients(); i++)
	{
		_sock.setSd(_sock.getClientSocket(i));
		if (FD_ISSET(_sock.getSd() , &_sock.getReadFds()))
		{
			// Check if it was for closing , and also read the
			// incoming message
			_sock.setValRead(read(_sock.getSd(), buffer, 1024));
			if (_sock.getValRead() == 0)
			{
				// Somebody disconnected , get his details and print
				getpeername(_sock.getSd(), (struct sockaddr*)&_sock.getHint(), (socklen_t*)&_sock.getClientSize());
				printf("Host disconnected , ip %s , port %d \n", inet_ntoa(_sock.getHint().sin_addr), ntohs(_sock.getHint().sin_port));
				// Close the socket and mark as 0 in list for reuse
				close(_sock.getSd());
				_sock.setClientSocket(i, 0);
				if (_clientHandler.finder(ntohs(_sock.getHint().sin_port)))
					quitCmd(_clientHandler.finder(ntohs(_sock.getHint().sin_port)));
			}
			// Echo back the message that came in
			else {
				try
				{
					interpreter(std::string(buffer, _sock.getValRead()), _sock.getSd());
				}
				catch(std::exception &error)
				{
					MSG(error.what());
				}
			}
		}
	}
}

void    Server::sockSet() {
	_sock.initSockets();
    _sock.bindSocket();
}

void    Server::interpreter(std::string const &msg, int const &sockFd) {
	std::string copy = msg;
    std::string cmd = msg.substr(0, msg.find(' '));
	Client		*client = _clientHandler.finder(sockFd);

	MSG(copy);
	copy.erase(copy.find('\r'), 2);
	if (!cmd.compare("QUIT"))
		quitCmd(client);
	else if (!cmd.compare("PRIVMSG"))
		privMsg(copy, client);
	else if (!cmd.compare("NICK"))
		setClientNick(copy, client);
	else if (!cmd.compare("KICK"))
		opKick(ft_split(copy, ' '), client);
	else if (!cmd.compare("MODE"))
		opMode(ft_split(copy, ' '), client);
	else if (!cmd.compare("TOPIC"))
		_channelHandler.opTopic(copy, client);
	else if (!cmd.compare("JOIN"))
		joinChannel(ft_split(copy, ' '), client);
	else if (!cmd.compare("INVITE"))
		inviteToChannel(ft_split(copy, ' '), client);
	else if (!cmd.compare("USER"))
		setClientUser(copy.substr(copy.find(' ') + 1), client);
	else if (!cmd.compare("PART"))
		partCmd(copy.substr(5, copy.find(' ', 5) - 5), client);
	else if (!cmd.compare("PASS"))
		_clientHandler.addClient(copy, _password, sockFd,
			std::string(inet_ntoa(_sock.getHint().sin_addr)), ntohs(_sock.getHint().sin_port));
}

// Check - and +
void Server::opMode(const std::vector<std::string> &msg, Client *chop) {
    std::string	args;
    Channel		*channel;
	std::string	flags = msg[2];

    if (!(channel = _channelHandler.finder(msg[1])))
        ERR_NOSUCHCHANNEL(msg[1], chop->getFd(), _errMsg);
    if (msg.size() < 3)
        ERR_NEEDMOREPARAMS(std::string("MODE"), chop->getFd(), _errMsg)
    for (long unsigned int i = 3; i < msg.size(); i++) {
        args += msg[i] + ' ';
	}
	if (!args.empty())
		args.erase(args.find(' '), 1);
	if (flags[1] == 'o' || flags[1] == 'b' || flags[1] == 'v') {
		if (!_clientHandler.finder(args)) {
			MSG("ERR no such user");
			return ;
		}
        channel->userMode(flags, _clientHandler.finder(args));
	}
	else
    	channel->cmdMode(msg[2], args, chop);
}

// ERR_NEEDMOREPARAMS
// ERR_USERONCHANNEL
// RPL_INVITING           RPL_AWAY
void	Server::inviteToChannel(const std::vector<std::string> &info, Client *inviter) {
	Client	*invited = _clientHandler.finder(info[1]);
	Channel	*channel = _channelHandler.finder(info[2]);

	if (!invited)
		ERR_NOSUCHNICK(info[1], inviter->getFd(), _errMsg);
	if (!channel) 
		ERR_NOSUCHCHANNEL(info[2],inviter->getFd(), _errMsg);
	if (!channel->usersOnChannel(inviter))
		ERR_NOTONCHANNEL(channel->getName(), inviter->getFd(), _errMsg);
	channel->cmdInvite(inviter, invited);
}

void    Server::joinChannel(const std::vector<std::string> &msg, Client *client) {
	// Limit of ten channels per user
	// User already in channel error
	// Join must come with the '#' or noSuchChannel ERR
	std::vector<std::string>::iterator	it;
	std::vector<std::string>			channels;
	int									fd = client->getFd();
	std::string							ip = client->getIp();
	std::string							nick = client->getNick();
	std::string							user = client->getUser();
	std::string 						channelMsg = msg[1].substr(msg[1].find(' ') + 1);

	if (msg.size() < 2) 
		ERR_NEEDMOREPARAMS(std::string("JOIN"), fd, _errMsg);
	channels = ft_split(channelMsg, ',');
	for (it = channels.begin(); it != channels.end(); it++) {
		if ((*it)[0] != '#')
			(*it).insert(0, 1, '#');
		if (Channel *channel = _channelHandler.finder(*it)) {
			std::string	joinMsg;
			if (channel->usersOnChannel(client))
				ERR_USERONCHANNEL(channel->getName(), client->getFd(), _errMsg);
			if (channel->retStateFlag('i', client))
				ERR_INVITEONLYCHAN(channel->getName(), fd, _errMsg);
			if (channel->retStateFlag('l') && ((channel->getUsersTotal() + 1) > channel->getLimit()))
				ERR_CHANNELISFULL(channel->getName(), fd, _errMsg);
			if (channel->finder(channel->getBan(), client))
				ERR_BANNEDFROMCHAN(channel->getName(), fd, _errMsg);
			if (channel->retStateFlag('k')) {
				if (msg.size() < 3)
					ERR_NEEDMOREPARAMS(channel->getName(), fd, _errMsg);
				if (msg[2] != channel->getPass())
					ERR_BADCHANNELKEY(channel->getName(), fd, _errMsg);
			}
			joinMsg = ':' + nick + '!' + user + '@' + ip + " JOIN " + (*it) + '\n';
			channel->sendMsgToUsers(joinMsg);
			channel->addClient(channel->getUsers(), client);
			client->addChannel(_channelHandler.finder((*it)));
		}
		else {
			_channelHandler.addChannel((*it), client);
			client->addChannel(_channelHandler.finder((*it)));
		}
		channelMsg = ":" + nick + "!" + user + "@" + ip + " JOIN :" + (*it) + "\n";
		channelMsg += "353 " + nick + " = " + (*it) + " :";
		channelMsg += _channelHandler.finder((*it))->getUsersString() + "\n";
		send(fd, channelMsg.c_str(), channelMsg.length(), 0);
		_channelHandler.finder((*it))->sendTopic(client);
	}
}

// ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
// ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
// ERR_CHANNELISFULL               ERR_BADCHANMASK
// ERR_TOOMANYCHANNELS
// RPL_TOPIC
void    Server::privMsg(const std::string &msg, Client *sender) {
	std::string sendMsg;

	if (msg.find('#') != std::string::npos) {
		Channel		*channel;
		std::string channelName = msg.substr(msg.find('#'), msg.find(':') - msg.find('#') - 1);

		if (!(channel = _channelHandler.finder(channelName)))
			ERR_NOSUCHCHANNEL(channelName, sender->getFd(), _errMsg);
		if (channel->retStateFlag('m') && (!channel->finder(channel->getModerator(), sender) || !channel->finder(channel->getChops(), sender))) {
			MSG("Moderated channel, user is not a moderator"); // Check in libera if it still prints on sender chat
			return ;
		}

		if (!sender->findChannel(channelName)) {
			MSG(sender->getNick() + " is not in this channel anymore");				// ERR
			return ;
		}
		sendMsg = ':' + sender->getNick();
		sendMsg += " PRIVMSG " + channelName + ' ' + msg.substr(msg.find(':')) + "\r\n";
		channel->sendMsgToUsers(sendMsg, sender->getFd());
	}
	else if (Client * client = _clientHandler.finder(msg.substr(8, msg.find(' ', 8) - 8))) {
		sendMsg = ":" + sender->getNick() + " PRIVMSG ";
		sendMsg += client->getNick() + ' ' + msg.substr(msg.find(':')) + "\r\n";
		send(client->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
	}
}

// ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
// ERR_NICKCOLLISION
void    Server::setClientNick(const std::string &msg, Client *client) { // Change nick in channel
	std::vector<Channel *>::iterator	it;
	std::string							sendMsg;
	std::vector<Channel *>				channels = client->getChannels();
	std::string							nick = msg.substr(5, msg.find('\0', 5));

	if (_clientHandler.finder(nick))
		ERR_NICKNAMEINUSE(nick, client->getFd(), _errMsg);
	if (client->getNick().empty()) {
		client->setNick(nick);
		return ;
	}
	sendMsg = ':' + client->getNick();
	sendMsg += " NICK " + nick + "\r\n";
	client->setNick(nick);
	send(client->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
	for (it = channels.begin(); it != channels.end(); it++)
		(*it)->sendMsgToUsers(sendMsg);
}

// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTRED
void    Server::setClientUser(const std::string &msg, Client *client) {
	std::string welcomeMsg;
	std::string user = msg.substr(0, msg.find(' '));
	std::string	realName = msg.substr(msg.find(':') + 1);

	client->setUser(user);
	client->setReal(realName);
	welcomeMsg = "\n001 " + client->getNick() + " :Welcome to **HiTeK** Server\r\n";
	send(client->getFd(), welcomeMsg.c_str(), welcomeMsg.length(), 0);
}

void	Server::opKick(const std::vector<std::string> &info, Client *kicker) {
	if (info.size() >= 2) {
        Channel *channel = _channelHandler.finder(info[1]);
        if (!channel)
            ERR_NOSUCHCHANNEL(info[1], kicker->getFd(), _errMsg);
        if (info.size() > 2) {
            channel->cmdKick(_clientHandler.finder(info[2]), kicker);
            return ;
        }
    }
    ERR_NEEDMOREPARAMS(std::string("KICK"), kicker->getFd(), _errMsg);
}

void	Server::partCmd(const std::string &channel, Client *parter) {
	if (Channel *part = _channelHandler.finder(channel)) {
		part->partChannel(parter);
		if (!part->usersOnChannel())
			_channelHandler.rmvChannel(channel);
	}
	else
		ERR_NOSUCHCHANNEL(channel, parter->getFd(), _errMsg);
}

void	Server::quitCmd(Client *quiter) {
	std::vector<Channel *>::iterator	it;
	std::vector<Channel *>				channels;

	if (!quiter->getChannels().empty()) {
		channels = quiter->getChannels();
		for (it = channels.begin(); it != channels.end(); it++) {
			(*it)->rmvClient(quiter);
			if (!(*it)->usersOnChannel())
				_channelHandler.rmvChannel((*it)->getName());
		}
	}
	_clientHandler.rmvClient(quiter->getNick());
}