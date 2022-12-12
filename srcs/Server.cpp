# include <vector>
# include <string>
# include <iostream>

#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Socket.hpp"


#define TEST(fd, toSend) send(fd, toSend.c_str(), toSend.length(), 0);

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

	copy.erase(msg.find('\r'), 2);
	MSG(msg);
	if (!cmd.compare("NICK"))
		setClientNick(msg, sockFd);
	else if (!cmd.compare("USER"))
		setClientUser(msg, sockFd);
	else if (!cmd.compare("JOIN"))
		joinChannel(msg, sockFd);
	else if (!cmd.compare("PART"))
	{
		std::vector<std::string> info = ft_split(copy);
		if (info.size() < 2)
		{
			std::string toSend = "461 PART :Not enough parameters\r\n";
			send(sockFd, toSend.c_str(), toSend.length(), 0);
		}
		Channel *channel = _channelHandler.finder(info[1]);
		if (!channel)
		{
			std::string toSend =  "403 " +  info[1] + " :No such channel\r\n";
			send(sockFd, toSend.c_str(), toSend.length(), 0);
		}
		else
			channel->partChannel(_clientHandler.finder(sockFd));
	}
	else if (!cmd.compare("PRIVMSG"))
		privMsg(msg, sockFd);
	else if (!cmd.compare("MODE"))
		_channelHandler.opMode(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("KICK"))
		_channelHandler.opKick(copy, _clientHandler.finder(sockFd)->getNick(), sockFd);
	else if (!cmd.compare("TOPIC"))
		_channelHandler.opTopic(copy, _clientHandler.finder(sockFd));
	else if (!cmd.compare("INVITE"))
	{
		// SEGFAULT IF USER NOT FOUND
		Channel						*channel;
		std::vector<std::string>	info = ft_split(copy);
		Client						*invited = _clientHandler.finder(info[1]);

		if (info[2][0] != '#')
			info[2].insert(0, 1, '#');
		channel = _channelHandler.finder(info[2]);
    	if (invited != NULL) {

			if (channel == NULL) {
				joinChannel("JOIN " + info[2] + "\r\n", sockFd);
				channel = _channelHandler.finder(info[2]);
			}
			std::cout << "INVITE\n";
			channel->cmdInvite(_clientHandler.finder(sockFd), invited);
		}
	}
	else if (!cmd.compare("PASS"))
		_clientHandler.addClient(msg, _password, sockFd, std::string(inet_ntoa(_sock.getHint().sin_addr)));
}

void    Server::joinChannel(const std::string &msg, const int &sockFd) {
	// JOIN can come with multiple channels. Parse the message in order to not create a channel 
	// wich has a name with all the names compiled -> split JOIN #c,#b,#c,#b,#c,#b,#c,#b

	// If a channel has more than one operator? 

	// See codes 403, 404, 405 --- 651 -- 366

	//When a user nickname changes, update on the participants list
	std::string copy = msg;
	copy.erase(msg.find('\r'), 2);
	std::vector<std::string> info = ft_split(copy);
	std::string channelMsg;			// Confirm message user/nick
	if (info.size() < 2)
	{
		channelMsg =  "461 JOIN :Not enough parameters\r\n";
		send(sockFd, channelMsg.c_str(), channelMsg.length(), 0);
		return ;
	}
	std::string channelName = info[1];
	std::string ip = _clientHandler.finder(sockFd)->getIp();
	std::string nick = _clientHandler.finder(sockFd)->getNick();
	std::string user = _clientHandler.finder(sockFd)->getUser();

	if (channelName[0] != '#')
		channelName.insert(0, 1, '#');
	if (Channel *channel = _channelHandler.finder(channelName)) {
		std::string	joinMsg;
		if (channel->retStateFlag('i'))
		{
			joinMsg =  "473 " +  channel->getName() + " :Cannot join channel (+i)\r\n";
			send(sockFd, joinMsg.c_str(), joinMsg.length(), 0);
			return ;
		}
		if (channel->retStateFlag('l') && ((channel->getUsersTotal() + 1) > channel->getLimit()))
		{

			joinMsg =  "471 " +  channel->getName() + " :Cannot join channel (+l)\r\n";
			send(sockFd, joinMsg.c_str(), joinMsg.length(), 0);
			return ;
		}
		if (channel->checkBan(nick))
		{
			joinMsg =  "474 " +  channel->getName() + " :Cannot join channel (+b)\r\n";
			send(sockFd, joinMsg.c_str(), joinMsg.length(), 0);
			return ;
		}
		if (channel->retStateFlag('k'))
		{
			if (info.size() < 3)
			{
				joinMsg =  "461 JOIN :Not enough parameters\r\n";
				send(sockFd, joinMsg.c_str(), joinMsg.length(), 0);
				return ;
			}
			if (info[2] != channel->getPass())
			{
				joinMsg =  "475 " + channel->getName() + " :Cannot join channel (+k)\r\n";
				send(sockFd, joinMsg.c_str(), joinMsg.length(), 0);
				return ;
			}
		}
		joinMsg = ':' + nick + '!' + user + '@' + ip + " JOIN " + channelName + '\n';
		channel->sendMsgToUsers(joinMsg);
		channel->addUser(_clientHandler.finder(sockFd));
	}
	else {
		_channelHandler.addChannel(channelName, _clientHandler.finder(sockFd));
		_clientHandler.finder(sockFd)->addChannel(_channelHandler.finder(channelName));
	}
	channelMsg = ":" + nick + "!" + user + "@" + ip + " JOIN :" + channelName + "\n";
	channelMsg += "353 " + nick + " = " + channelName + " :";
	channelMsg += _channelHandler.finder(channelName)->getUsersString() + "\n";		// Send list of users. What if there are multiple operators?
	send(sockFd, channelMsg.c_str(), channelMsg.length(), 0);
	_channelHandler.finder(channelName)->sendTopic(_clientHandler.finder(sockFd));
}

void    Server::privMsg(const std::string &msg, const int &sockFd) {
	// If nick not found Exception
	// See codes - 412, 717, 
	std::string sendMsg;

	if (msg.find('#') != std::string::npos) {
		std::string channelName = msg.substr(msg.find('#'), msg.find(':') - msg.find('#') - 1);

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

void    Server::setClientNick(const std::string &msg, const int &sockFd) {
	std::string	nick = msg.substr(5, msg.find('\0', 5));

	nick.erase(nick.find('\r'), 2);
	_clientHandler.finder(sockFd)->setNick(nick);
	nick = "001 " + nick + "\n";
	send(sockFd, nick.c_str(), nick.length(), 0);
}

void    Server::setClientUser(const std::string &msg, const int &sockFd) {
	std::string	user = msg.substr(msg.find(':') + 1);

	user.erase(user.find('\r'), 2);
	_clientHandler.finder(sockFd)->setUser(user);
}