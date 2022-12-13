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
	if (!cmd.compare("PRIVMSG"))
		privMsg(copy, sockFd);
	else if (!cmd.compare("JOIN"))
		joinChannel(ft_split(copy, ' '), sockFd);
	else if (!cmd.compare("NICK"))
		setClientNick(copy, sockFd);
	else if (!cmd.compare("USER"))
		setClientUser(copy, sockFd);
	else if (!cmd.compare("INVITE"))
		inviteToChannel(ft_split(copy, ' '), sockFd);
	else if (!cmd.compare("PART"))								// If operator leaves there is no succession
		partCmd(copy.substr(5, copy.find(' ', 5) - 5), sockFd);
	else if (!cmd.compare("MODE"))
		_channelHandler.opMode(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("KICK"))
		opKick(ft_split(copy, ' '), _clientHandler.finder(sockFd)->getNick(), sockFd);
	else if (!cmd.compare("TOPIC"))
		_channelHandler.opTopic(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("PASS"))
		_clientHandler.addClient(copy, _password, sockFd, std::string(inet_ntoa(_sock.getHint().sin_addr)));
}

void	Server::inviteToChannel(const std::vector<std::string> &info, const int &sockFd) {
	// INVITE  - Invite a client to an invite-only channel (mode +i), should it be just on this ocasion?
	// On the mode +i there should be a flag indicating wich users where invited?
	Client	*invited = _clientHandler.finder(info[1]);
	Channel	*channel = _channelHandler.finder(info[2]);

	if (invited != NULL) {													// missing invited user error?
		if (channel == NULL) {
			std::string toSend = "JOIN " + info[2];

			joinChannel(ft_split(toSend, ' '), sockFd);
			channel = _channelHandler.finder(info[2]);
		}
		std::cout << "INVITE\n";
		channel->cmdInvite(_clientHandler.finder(sockFd), invited);
	}
}

void    Server::joinChannel(const std::vector<std::string> &msg, const int &sockFd) {
	// JOIN can come with multiple channels. Parse the message in order to not create a channel 
	// wich has a name with all the names compiled -> split JOIN #c,#b,#c,#b,#c,#b,#c,#b
	// Channel name should not contain spaces
	// Limit of ten channels per user
	// If a channel has more than one operator? 
	// See codes 403, 404, 405 --- 651 -- 366
	//When a user nickname changes, update on the participants list
	if (msg.size() < 2)
		ERR_NEEDMOREPARAMS(std::string("JOIN"), sockFd, std::string(""));
	std::string ip = _clientHandler.finder(sockFd)->getIp();
	std::string nick = _clientHandler.finder(sockFd)->getNick();
	std::string user = _clientHandler.finder(sockFd)->getUser();

	std::vector<std::string>::iterator	it;
	std::string 						channelMsg = msg[2].substr(msg[2].find(' ') + 1);
	std::vector<std::string>			channels = ft_split(channelMsg, ',');

	for (it = channels.begin(); it != channels.end(); it++) {
		if ((*it)[0] != '#')
			(*it).insert(0, 1, '#');
		if (Channel *channel = _channelHandler.finder(*it)) {
			std::string	joinMsg;

			if (channel->retStateFlag('i'))
				ERR_INVITEONLYCHAN(channel->getName(), sockFd, std::string(""));
			if (channel->retStateFlag('l') && ((channel->getUsersTotal() + 1) > channel->getLimit()))
				ERR_CHANNELISFULL(channel->getName(), sockFd, std::string(""));
			if (channel->checkBan(nick))
				ERR_BANNEDFROMCHAN(channel->getName(), sockFd, std::string(""));
			if (channel->retStateFlag('k')) {
				if (msg.size() < 3)
					ERR_NEEDMOREPARAMS(channel->getName(), sockFd, std::string(""));
				if (msg[2] != channel->getPass())
					ERR_BADCHANNELKEY(channel->getName(), sockFd, std::string(""));
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
			MSG(_clientHandler.finder(sockFd)->getNick() + " is not in this channel anymore");
			return ;
		}
		if (Channel *channel = _channelHandler.finder(channelName)) {
			sendMsg = ':' + _clientHandler.finder(sockFd)->getNick();
			sendMsg += " PRIVMSG " + channelName + ' ' + msg.substr(msg.find(':'));
			channel->sendMsgToUsers(sendMsg, sockFd);
		}
	}
	else if (Client * client = _clientHandler.finder(msg.substr(8, msg.find(' ', 8) - 8))) {
		sendMsg = ":" + _clientHandler.finder(sockFd)->getNick() + " PRIVMSG ";
		sendMsg += client->getNick() + ' ' + msg.substr(msg.find(':'));
		send(client->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
	}
}

void    Server::setClientNick(const std::string &msg, const int &sockFd) { // Change nick in channel
	std::string	nick = msg.substr(5, msg.find('\0', 5));

	//nick.erase(nick.find('\r'), 2);
	_clientHandler.finder(sockFd)->setNick(nick);
	nick = "001 " + nick + "\n";
	send(sockFd, nick.c_str(), nick.length(), 0);
}

void    Server::setClientUser(const std::string &msg, const int &sockFd) {
	std::string	user = msg.substr(msg.find(':') + 1);
	_clientHandler.finder(sockFd)->setUser(user);
}

void	Server::opKick(const std::vector<std::string> &info, const std::string &chop, const int &fd) {    // User not foud error. Kick wait time
	if (info.size() >= 2)
    {
        Channel *channel = _channelHandler.finder(info[1]);
        if (!channel)
            ERR_NOSUCHCHANNEL(info[1], fd, std::string(""));
        if (info.size() > 2) {
            channel->cmdKick(_clientHandler.finder(info[2]), chop, fd);
            return ;
        }
    }
    ERR_NEEDMOREPARAMS(std::string("KICK"), fd, std::string(""));
}

void	Server::partCmd(const std::string &channel, const int &fd) {
	if (Channel *part = _channelHandler.finder(channel))
		part->partChannel(_clientHandler.finder(fd));
	else
		ERR_NOSUCHCHANNEL(channel, fd, std::string(""));
}