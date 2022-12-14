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
		// Send new connection greeting message
		/*if (send(_sock.getNewSocket(), message.c_str(), strlen(message.c_str()), 0) != strlen(message.c_str()))
		{
			MSG("Sending Message Error");
			throw ConnectionException();
		}*/
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
    std::string cmd = msg.substr(0, msg.find(' '));
	std::string copy = msg;

	copy.erase(copy.find('\r'), 2);
	MSG(copy);
	if (!cmd.compare("QUIT"))
		quitCmd(_clientHandler.finder(sockFd));
	if (!cmd.compare("PRIVMSG"))
		privMsg(copy, sockFd);
	else if (!cmd.compare("JOIN"))
		joinChannel(ft_split(copy, ' '), sockFd);
	else if (!cmd.compare("NICK"))
		setClientNick(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("USER"))
		setClientUser(copy.substr(copy.find(' ') + 1), _clientHandler.finder(sockFd));
	else if (!cmd.compare("INVITE"))       
		inviteToChannel(ft_split(copy, ' '), sockFd);
	else if (!cmd.compare("PART"))
		partCmd(copy.substr(5, copy.find(' ', 5) - 5), sockFd);
	else if (!cmd.compare("MODE"))
		_channelHandler.opMode(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("TOPIC"))
		_channelHandler.opTopic(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("KICK"))
		opKick(ft_split(copy, ' '), _clientHandler.finder(sockFd)->getNick(), sockFd);
	else if (!cmd.compare("PASS"))
		_clientHandler.addClient(copy, _password, sockFd, std::string(inet_ntoa(_sock.getHint().sin_addr)), ntohs(_sock.getHint().sin_port));
}

void	Server::inviteToChannel(const std::vector<std::string> &info, const int &sockFd) {

	if (info.size() < 3)
		ERR_NEEDMOREPARAMS(info[0], sockFd, _errMsg);
	Client	*invited = _clientHandler.finder(info[1]);
	Channel	*channel = _channelHandler.finder(info[2]);

	if (invited != NULL) {
		if (channel == NULL) {
			std::string toSend = "JOIN " + info[2];
			joinChannel(ft_split(toSend, ' '), sockFd);
			channel = _channelHandler.finder(info[2]);
		}
		if (!channel->usersOnChannel(_clientHandler.finder(sockFd)->getNick()))
			ERR_NOTONCHANNEL(channel->getName(), sockFd, _errMsg);
		if (!channel->finder(channel->getChop(), _clientHandler.finder(sockFd)->getNick()))
			ERR_CHANOPRIVSNEEDED(channel->getName(), sockFd, _errMsg);
		std::cout << "INVITE\n";
		channel->cmdInvite(_clientHandler.finder(sockFd), invited);
	}
	else
		ERR_NOSUCHNICK(info[1], sockFd, _errMsg);
}


// ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
// ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
// ERR_CHANNELISFULL               ERR_BADCHANMASK
// ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
// RPL_TOPIC
void    Server::joinChannel(const std::vector<std::string> &msg, const int &sockFd) { // Put Client *
	// Limit of ten channels per user
	// User already in channel error

	if (msg.size() < 2) 
		ERR_NEEDMOREPARAMS(std::string("JOIN"), sockFd, _errMsg);

	std::string 						channelMsg = msg[1].substr(msg[1].find(' ') + 1);
	std::string							nick = _clientHandler.finder(sockFd)->getNick();
	std::string							user = _clientHandler.finder(sockFd)->getUser();
	std::string							ip = _clientHandler.finder(sockFd)->getIp();
	std::vector<std::string>			channels = ft_split(channelMsg, ',');
	std::vector<std::string>::iterator	it;

	for (it = channels.begin(); it != channels.end(); it++) {
		if ((*it)[0] != '#' && (*it)[0] != '&')
			(*it).insert(0, 1, '#');
		if (Channel *channel = _channelHandler.finder(*it)) {
			std::string	joinMsg;

			if (channel->finder(channel->getChops(), nick) || channel->finder(channel->getUsers(), nick) || channel->finder(channel->getMuted(), nick))
				return ;
			if (channel->retStateFlag('i', _clientHandler.finder(sockFd)->getNick()))
				ERR_INVITEONLYCHAN(channel->getName(), sockFd, _errMsg);
			if (channel->retStateFlag('l') && ((channel->getUsersTotal() + 1) > channel->getLimit()))
				ERR_CHANNELISFULL(channel->getName(), sockFd, _errMsg);
			if (channel->checkBan(nick))
				ERR_BANNEDFROMCHAN(channel->getName(), sockFd, _errMsg);
			if (channel->retStateFlag('k')) {
				if (msg.size() < 3)
					ERR_NEEDMOREPARAMS(channel->getName(), sockFd, _errMsg);
				if (msg[2] != channel->getPass())
					ERR_BADCHANNELKEY(channel->getName(), sockFd, _errMsg);
			}
			joinMsg = ':' + nick + '!' + user + '@' + ip + " JOIN " + (*it) + '\n';
			channel->sendMsgToUsers(joinMsg);
			if (channel->getChops().empty())
				channel->addChop(_clientHandler.finder(sockFd));
			else
				channel->addUser(_clientHandler.finder(sockFd));
			_clientHandler.finder(sockFd)->addChannel(_channelHandler.finder((*it)));
		}
		else {
			_channelHandler.addChannel((*it), _clientHandler.finder(sockFd));
			_clientHandler.finder(sockFd)->addChannel(_channelHandler.finder((*it)));
		}
		channelMsg = ":" + nick + "!" + user + "@" + ip + " JOIN :" + (*it) + "\n";
		channelMsg += "353 " + nick + " = " + (*it) + " :";
		channelMsg += _channelHandler.finder((*it))->getUsersString() + "\n";
		send(sockFd, channelMsg.c_str(), channelMsg.length(), 0);
		_channelHandler.finder((*it))->sendTopic(_clientHandler.finder(sockFd));
	}
}


void    Server::privMsg(const std::string &msg, const int &sockFd) {
	// If nick not found Exception
	// See codes - 412, 717, 
	std::string sendMsg;

	if (msg.find('#') != std::string::npos) {
		std::string channelName = msg.substr(msg.find('#'), msg.find(':') - msg.find('#') - 1);

		if (!_clientHandler.finder(sockFd)->findChannel(channelName)) {
			MSG(_clientHandler.finder(sockFd)->getNick() + " is not in this channel anymore");				// ERR
			return ;
		}
		if (Channel *channel = _channelHandler.finder(channelName)) {
			sendMsg = ':' + _clientHandler.finder(sockFd)->getNick();
			sendMsg += " PRIVMSG " + channelName + ' ' + msg.substr(msg.find(':')) + "\r\n";
			channel->sendMsgToUsers(sendMsg, sockFd);
		}
	}
	else if (Client * client = _clientHandler.finder(msg.substr(8, msg.find(' ', 8) - 8))) {
		sendMsg = ":" + _clientHandler.finder(sockFd)->getNick() + " PRIVMSG ";
		sendMsg += client->getNick() + ' ' + msg.substr(msg.find(':')) + "\r\n";
		send(client->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
	}
}

// ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
// ERR_NICKNAMEINUSE               ERR_NICKCOLLISION
void    Server::setClientNick(const std::string &msg, Client *client) { // Change nick in channel
	std::vector<Channel *>::iterator	it;
	std::string							sendMsg;
	std::vector<Channel *>				channels = client->getChannels();
	std::string							nick = msg.substr(5, msg.find('\0', 5));

	if (_clientHandler.finder(nick))						// ERR
		ERR_NICKNAMEINUSE(nick, client->getFd(), _errMsg);
	if (client->getNick().empty()) {	
		client->setNick(nick);
		return ;
	}
	sendMsg = ':' + client->getNick();
	sendMsg += " NICK " + nick + '\n';
	client->setNick(nick);
	send(client->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
	for (it = channels.begin(); it != channels.end(); it++)
		(*it)->sendMsgToUsers(sendMsg);
}

// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTRED
void    Server::setClientUser(const std::string &msg, Client *client) {   // Exception if there is no nick or warever
	std::string welcomeMsg;
	std::string user = msg.substr(0, msg.find(' '));
	std::string	realName = msg.substr(msg.find(':') + 1);

	client->setUser(user);
	client->setReal(realName);
	welcomeMsg = "001 " + client->getNick() + " :Welcome to **HiTeK** Server\n";
	send(client->getFd(), welcomeMsg.c_str(), welcomeMsg.length(), 0);
}

void	Server::opKick(const std::vector<std::string> &info, const std::string &chop, const int &fd) {
	if (info.size() >= 2) {
        Channel *channel = _channelHandler.finder(info[1]);
        if (!channel)
            ERR_NOSUCHCHANNEL(info[1], fd, _errMsg);
        if (info.size() > 2) {
            channel->cmdKick(_clientHandler.finder(info[2]), chop, fd);
            return ;
        }
    }
    ERR_NEEDMOREPARAMS(std::string("KICK"), fd, _errMsg);
}

void	Server::partCmd(const std::string &channel, const int &fd) {
	if (Channel *part = _channelHandler.finder(channel)) {
		part->partChannel(_clientHandler.finder(fd));
		if (!part->usersOnChannel())
			_channelHandler.rmvChannel(channel);
	}
	else
		ERR_NOSUCHCHANNEL(channel, fd, _errMsg);
}

void	Server::quitCmd(Client *quiter) {
	_clientHandler.rmvClient(quiter->getNick());
}