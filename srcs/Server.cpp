# include <vector>
# include <string>
# include <iostream>

#include "../include/Utils.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Socket.hpp"
#include "../include/Macros.hpp"


/**
 * @brief Handle a new connection in the "master" socket and add it to the array of sockets
 */
void Server::new_connection()
{
	_sock.setNewSocket(accept(_sock.getSocketFd(), (struct sockaddr *)&_sock.getHint(), (socklen_t*)&_sock.getClientSize()));
	if (_sock.getNewSocket() < 0)
	{
		MSG("Accepting Error");
		throw ConnectionException();
	}
	// Inform user of socket number - used in send and receive commands   -- WE DON'T NEED THIS. JUST FOR DEBUG
	std::cout << "New connection , socket fd is " << _sock.getNewSocket() 
		<< ", IP is: " << inet_ntoa(_sock.getHint().sin_addr) 
			<< ", port: " << ntohs(_sock.getHint().sin_port) << std::endl;
	_clientHandler.addClient(_sock.getNewSocket(), std::string(inet_ntoa(_sock.getHint().sin_addr)), ntohs(_sock.getHint().sin_port));
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

/**
 * @brief 			Handle each input and output operation (receive and send messages)
 * @param buffer    Message to read
 * @param i    		socket index
 */
void	Server::io_operations(char *buffer, int i)
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
				std::string cmd;
				cmd = _clientHandler.processCmd(std::string(buffer, _sock.getValRead()), _sock.getSd());
				if (!cmd.empty())
					interpreter(cmd, _sock.getSd());
			}
			catch(std::exception &error)
			{
				MSG(error.what());
			}
		}
	}
}

/**
 * @brief Main function to handle connections and activities
 */
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
		new_connection();
	// Else its some IO operation on some other socket
	for (int i = 0; i < _sock.getMaxClients(); i++)
		io_operations(buffer, i);
}

/**
 * @brief Initiates and binds Server _socket
 */
void    Server::sockSet() {
	_sock.initSockets();
    _sock.bindSocket();
}

/**
 * @brief			Searches messages coming from _socket for commands and executes them
 * @param msg		Message to interprete
 * @param sockFd	Client fd
 */
void    Server::interpreter(const std::string &msg, int const &sockFd) 
{
	MSG(msg);
	std::string copy = msg;
	Client		*client = _clientHandler.finder(sockFd);

	if (copy[copy.length() - 1] == '\n')
		copy.erase(copy.size() - 1, 1);
	std::vector<std::string> commands = ft_split(copy, '\n');
	std::vector<std::string>::iterator it;
	for (it = commands.begin(); it != commands.end(); it++)
	{
		if ((*it).find('\r') != std::string::npos)
			(*it).erase((*it).find('\r'), 1);
	}

	size_t i = -1;
	while (++i < commands.size())
	{
		std::vector<std::string> args = ft_split(commands[i], ' ');

		if (!client->getRegistration())
		{
			if (args[0].compare("PASS") && args[0].compare("NICK") && args[0].compare("USER") && args[0].compare("CAP"))
				ERR_NOTREGISTERED(args[0], sockFd, _errMsg)
		}
		if (!args[0].compare("QUIT"))
			quitCmd(client);
		else if (!args[0].compare("PING"))
			pong(client);
		else if (!args[0].compare("OPER"))
			operCmd(args, client);
		else if (!args[0].compare("KILL"))
			killCmd(commands[i], client);
		else if (!args[0].compare("KICK"))
			opKick(commands[i], client);
		else if (!args[0].compare("MODE"))
			opMode(args, client);
		else if (!args[0].compare("PART"))
			partCmd(args, client);
		else if (!args[0].compare("JOIN"))
			joinChannel(args, client);
		else if (!args[0].compare("NICK"))
			setClientNick(args, client);
		else if (!args[0].compare("USER"))
			setClientUser(args, client);
		else if (!args[0].compare("INVITE"))
			inviteToChannel(args, client);
		else if (!args[0].compare("TOPIC"))
			opTopic(args, client);
		else if (!args[0].compare("PRIVMSG") || !args[0].compare("NOTICE"))
			privMsg(args, client);
		else if (!args[0].compare("PASS"))
		{
			if (!_password.compare(args[1]))
				client->setPass();
		}
	}
}

/**
 * @brief			Parses the kill message, searches for the client to kill and sends kill message
 * @param args		String with the nick of the client to kill and arguments to use as motive
 * @param killer	Client issuing the /kill command
 */
void Server::opTopic(std::vector<std::string> msg, Client *client) {
    
	Channel		*channel;

	// Basic Error Handling
	if ( (msg.size() < 2) ) // check this with nc
        ERR_NEEDMOREPARAMS(std::string("TOPIC"), client->getFd(), _errMsg)
    if (!(channel = _channelHandler.finder(msg[1])))
        ERR_NOSUCHCHANNEL(msg[1], client->getFd(), _errMsg)
	if (!channel->usersOnChannel(client))
		ERR_NOTONCHANNEL(channel->getName(), client->getFd(), _errMsg)
	// Reply with topic
	if (msg.size() == 2)
	{
		channel->sendTopic(client);
		return ;
	}

	// Set the topic
	if ( (channel->retStateFlag('t')) && (!channel->finder(channel->getChops(), client)))
        ERR_CHANOPRIVSNEEDED(channel->getName(), client->getFd(), _errMsg);
	// Join the topic in a string and erase the last space in args
	std::string args;
    for (size_t i = 2; i < msg.size(); i++) {
        args += msg[i] + ' ';
	}
	if (!args.empty())
		args.erase(args.find_last_of(' '), 1);
	
	channel->cmdTopic(args, client);
}


/**
 * @brief			Parses the kill message, searches for the client to kill and sends kill message
 * @param args		String with the nick of the client to kill and arguments to use as motive
 * @param killer	Client issuing the /kill command
 */
void    Server::killCmd(const std::string &args, Client *killer) {
	Client		*killed;
	std::string	sendMsg;
	std::string	argsMsg;

	if (!operFinder(killer))
		ERR_NOPRIVILEGES(killer->getNick(), killer->getFd(), _errMsg)
	if (args.size() < 2)
        ERR_NEEDMOREPARAMS(std::string("OPER"), killer->getFd(), _errMsg)
	if (!(killed = _clientHandler.finder(args.substr(5, args.find(" :") - 5))))
		ERR_NOSUCHNICK(args.substr(5, args.find(" :") - 5), killer->getFd(), _errMsg)
	if (args.size() >= 3)
		sendMsg = ':' + killer->getNick() + ' ' + args + "\r\nERROR :Closing Link:\r\n";
	else if (args.size() == 2)
		sendMsg = ':' + killer->getNick() + ' ' + args + "\r\nERROR :Closing Link:\r\n";
	SEND(killed->getFd(), sendMsg)
}

/**
 * @brief			Checks if the username and password for the /oper command are correct and grants operator privileges
 * @param args		String vector with the username and passowrd required to become operator
 * @param client	Client issuing the /oper command
 */
void    Server::operCmd(const std::vector<std::string> &args, Client *client) {

    if (args.size() < 3)
        ERR_NEEDMOREPARAMS(std::string("OPER"), client->getFd(), _errMsg)
	if (!_opUserName.compare(args[1]))
		if (!_opPassword.compare(args[2])) {
			addOper(client);
			RPL_YOUREOPER(client->getFd(), _errMsg)
		}
	ERR_PASSWDMISMATCH(client->getNick(), client->getFd(), _errMsg)
}

/**
 * @brief       Simple error handling for MODE cmd and parse the flag used
 * @param msg   Vector with the Command and Parameters
 * @param chop  Channel Operator Pointer
**/
void Server::opMode(const std::vector<std::string> &msg, Client *chop) {

    std::string	args;
    Channel		*channel;

	// Basic Error Handling
	if ( (msg.size() < 3) || (msg[2].size() < 2) )
        ERR_NEEDMOREPARAMS(std::string("MODE"), chop->getFd(), _errMsg)
    if (!(channel = _channelHandler.finder(msg[1])))
        ERR_NOSUCHCHANNEL(msg[1], chop->getFd(), _errMsg)
	if (!channel->finder(channel->getChops(), chop))
        ERR_CHANOPRIVSNEEDED(channel->getName(), chop->getFd(), _errMsg);

	// Join the args in a string and erase the last space in args
    for (size_t i = 3; i < msg.size(); i++) {
        args += msg[i] + ' ';
	}
	if (!args.empty())
		args.erase(args.find_last_of(' '), 1);

	// Parse by the flag
	std::string	flags = msg[2];
	if (flags[1] == 'o' || flags[1] == 'b' || flags[1] == 'v') {
		if ( (msg.size() < 4))
        	ERR_NEEDMOREPARAMS(std::string("MODE"), chop->getFd(), _errMsg)
		if (!_clientHandler.finder(args))
			ERR_NOSUCHNICK(chop->getNick(), chop->getFd(), _errMsg)
        channel->userMode(flags, _clientHandler.finder(args), chop);
	}
	else
    	channel->cmdMode(msg[2], args, chop);
}

/**
 * @brief			Parse the invite command, error handlig and send invite message 
 * @param args		String vector with the INVITE command, targets and the channel
 * @param inviter	Client issuing the /invite command
 */
void	Server::inviteToChannel(std::vector<std::string> args, Client *inviter) {

	Client *invited;
	Channel	*channel;

	if (args.size() < 3)
		ERR_NEEDMOREPARAMS(args[0], inviter->getFd(), _errMsg)
	if (!(channel = _channelHandler.finder(args[2])))
		ERR_NOSUCHCHANNEL(args[2],inviter->getFd(), _errMsg)
	if (!channel->usersOnChannel(inviter))
		ERR_NOTONCHANNEL(channel->getName(), inviter->getFd(), _errMsg)
	if (!(invited = _clientHandler.finder(args[1])))
		ERR_NOSUCHNICK(args[1], inviter->getFd(), _errMsg)
	if (channel->usersOnChannel(invited))
		ERR_USERONCHANNEL(channel->getName(), inviter->getFd(), _errMsg)
	channel->cmdInvite(inviter, invited);
}

// RPL_TOPIC
// ERR_BADCHANMASK
// ERR_TOOMANYCHANNELS
void    Server::joinChannel(const std::vector<std::string> &msg, Client *client) {

	MSG("Join: ." + msg[0] + ".");

	std::vector<std::string>::iterator	it;
	std::vector<std::string>			channels;
	int									fd = client->getFd();
	std::string							ip = client->getIp();
	std::string							nick = client->getNick();
	std::string							user = client->getUser();
	std::string 						channelMsg = msg[1].substr(msg[1].find(' ') + 1);

	if (msg.size() < 2) 
		ERR_NEEDMOREPARAMS(std::string("JOIN"), fd, _errMsg)
	channels = ft_split(channelMsg, ',');
	for (it = channels.begin(); it != channels.end(); it++) {
		if ((*it)[0] != '#')
			ERR_NOSUCHCHANNEL_CONT((*it), client->getFd(), _errMsg)
		if (Channel *channel = _channelHandler.finder(*it)) {
			std::string	joinMsg;
			if (channel->usersOnChannel(client))
				ERR_USERONCHANNEL_CONT(channel->getName(), client->getFd(), _errMsg)
			if (channel->retStateFlag('i', client, channel->getInvited()))
				ERR_INVITEONLYCHAN_CONT(channel->getName(), fd, _errMsg)
			if (channel->retStateFlag('l') && ((channel->getUsersTotal() + 1) > channel->getLimit()))
				ERR_CHANNELISFULL_CONT(channel->getName(), fd, _errMsg)
			if (channel->finder(channel->getBan(), client))
				ERR_BANNEDFROMCHAN_CONT(channel->getName(), fd, _errMsg)
								MSG("DEBUG KEY");
			if (channel->retStateFlag('k')) {

				if (msg.size() < 3)
					ERR_NEEDMOREPARAMS_CONT(channel->getName(), fd, _errMsg)
				if (msg[2] != channel->getPass())
					ERR_BADCHANNELKEY_CONT(channel->getName(), fd, _errMsg)
			}
			joinMsg = ':' + nick + '!' + user + '@' + ip + " JOIN " + (*it) + "\r\n";
			channel->sendMsgToUsers(joinMsg);
			channel->addClient(channel->getUsers(), client);
			client->addChannel(_channelHandler.finder((*it)));
		}
		else {
			_channelHandler.addChannel((*it), client);
			client->addChannel(_channelHandler.finder((*it)));
		}
		channelMsg = ":" + nick + "!" + user + "@" + ip + " JOIN :" + (*it) + "\r\n";
		SEND(fd, channelMsg)
		channelMsg = "353 " + nick + " = " + (*it) + " :";
		channelMsg += _channelHandler.finder((*it))->getUsersString() + "\r\n";
		SEND(fd, channelMsg)
		_channelHandler.finder((*it))->sendTopic(client);
	}
}

// ERR_NOTOPLEVEL
// ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
// RPL_AWAY
/**
 * @brief			PRIVMSG and NOTICE command -> Sends messages to targets of /msg command
 * @param info		String vector with the PRIVMSG command, targets and the message to send
 * @param sender	Client issuing the /msg command
 */
void    Server::privMsg(const std::vector<std::string> &info, Client *sender) {
	std::vector<std::string>::iterator	it;
	std::string							msg;
	std::string							cmd;
	std::string							sendMsg;
	std::vector<std::string>			targets;

	if (info.size() == 1)
		ERR_NORECIPIENT(sender->getFd(), _errMsg);
	if (info.size() == 2)
		ERR_NEEDMOREPARAMS(info[0], sender->getFd(), _errMsg);
	if (info[2].length() == 1)	// Check in libera if this error is if msg is only ":" or if info[2] doesn't exist .. Needs to be tested with nc .. weechat always puts the ':'
		ERR_NOTEXTTOSEND(info[1], sender->getFd(), _errMsg)
	if (info.size() > 3) {
		msg = info[2];
		for(size_t i = 3; i < info.size(); i++)
			msg += ' ' + info[i];
	}
 	(!info[0].compare("PRIVMSG")) ? cmd = " PRIVMSG " : cmd = " NOTICE ";
	privMsgLoop(ft_split(info[1], ','), info[2], cmd, sender);
}

/**
 * @brief			PRIVMSG and NOTICE command send loop
 * @param targets	String vector with the targets to send the message
 * @param msg		Message to send
 * @param cmd		Command issued PRIVMSG/NOTICE
 * @param sender	Client issuing the /msg command
 */
void	Server::privMsgLoop(std::vector<std::string> targets, const std::string &msg,
							const std::string &cmd, Client *sender) {
	std::vector<std::string>::iterator	it;
	std::string							sendMsg;

	for (it = targets.begin(); it != targets.end(); it++) {
		if ((*it)[0] == '#')
		{
			Channel		*channel;
			if (!(channel = _channelHandler.finder((*it))))
				ERR_NOSUCHCHANNEL((*it), sender->getFd(), _errMsg);
			if (channel->finder(channel->getBan(), sender) && !channel->finder(channel->getChop(), sender))
				ERR_BANNEDFROMCHAN_CONT(channel->getName(), sender->getFd(), _errMsg)
			if (msg.length() == 1)	// Check in libera if this error is if msg is only ":" or if msg doesn't exist
				ERR_NOTEXTTOSEND(channel->getName(), sender->getFd(), _errMsg)
			if (channel->retStateFlag('m'))
			{
				if (!channel->finder(channel->getModerator(), sender) && !channel->finder(channel->getChops(), sender))
					ERR_CANNOTSENDTOCHAN(channel->getName(), sender->getFd(), _errMsg); // Check in libera if it still prints on sender chat
			}
			if (!sender->findChannel((*it)) && channel->retStateFlag('n')) 
				ERR_CANNOTSENDTOCHAN(channel->getName(), sender->getFd(), _errMsg);
			std::string channel_name = msg;
			sendMsg = ':' + sender->getNick();
			sendMsg += cmd + (*it) + ' ' + channel_name.erase(0, 1) + "\r\n";
			channel->sendMsgToUsers(sendMsg, sender->getFd());
		}
		else if (Client * client = _clientHandler.finder((*it))) {
			std::string channel_name = msg;
			sendMsg = ":" + sender->getNick() + cmd;
			sendMsg += client->getNick() + ' ' + channel_name.erase(0, 1) + "\r\n";
			send(client->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
		}
		else
			ERR_NOSUCHNICK((*it), sender->getFd(), _errMsg);
	}
}

/**
 * @brief			Sets/unsets the client nickname
 * @param msg		String vector with the NICK command and the desired nickname
 * @param client	Client issuing the /nick command
 */
void    Server::setClientNick(std::vector<std::string> msg, Client *client) {
	std::vector<Channel *>::iterator	it;
	std::string							sendMsg;
	std::vector<Channel *>				channels = client->getChannels();
	std::string							nick = msg[1];

	if (msg.size() < 2)
		ERR_NONICKNAMEGIVEN(client->getNick(), client->getFd(), _errMsg)
	if (_clientHandler.finder(nick))
		ERR_NICKNAMEINUSE(nick, client->getFd(), _errMsg);
	if (nick.find(',') != std::string::npos || nick.find('.') != std::string::npos)
		ERR_ERRONEUSNICKNAME(client->getNick(), nick, client->getFd(), _errMsg)
	if (client->getNick().empty()) {
		setNick(nick, client);
		return ;
	}
	sendMsg = ':' + client->getNick();
	sendMsg += " NICK " + nick + "\r\n";
	client->setNick(nick);
	SEND(client->getFd(), sendMsg)
	for (it = channels.begin(); it != channels.end(); it++)
		(*it)->sendMsgToUsers(sendMsg);
}

/**
 * @brief			Sets the nickname on the register
 * @param nick		Desired nickname
 * @param client	Client registering
 */
void	Server::setNick(const std::string &nick, Client *client) {
	if (_clientHandler.finder(nick))
			ERR_NICKNAMEINUSE(nick, client->getFd(), _errMsg);
		client->setNick(nick);
		if (!client->getUser().empty()) {
			client->setRegistration();
			std::string welcomeMsg = "001 " + client->getNick() + " :Welcome to '**HiTeK**' Server\r\n";
			send(client->getFd(), welcomeMsg.c_str(), welcomeMsg.length(), 0);
		}
}

// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTRED
void    Server::setClientUser(std::vector<std::string> args, Client *client) {
	std::string welcomeMsg;

	client->setUser(args[1]);
	client->setReal(args[2]);
	if (client->getPass())
	{
		if (!client->getNick().empty())
		{
			client->setRegistration();
			welcomeMsg = "001 " + client->getNick() + " :Welcome to '**HiTeK**' Server\r\n";
			send(client->getFd(), welcomeMsg.c_str(), welcomeMsg.length(), 0);
		}
	}
	else
	{
		ERR_PASSWDMISMATCH(client->getNick(), client->getFd(), _errMsg)
	}

}

/**
 * @brief			Parses the kick command, checks if kicked user exists and sends the parsed command to channel kick function
 * @param args		String vector with the nickname and reason for the kick
 * @param client	Client issuing the /kick command
 */
void	Server::opKick(const std::string &args, Client *kicker) {
	std::vector<std::string>	info = ft_split(args, ' ');
	Client						*kicked = _clientHandler.finder(info[2]);

	if (info.size() >= 2) {
        Channel *channel = _channelHandler.finder(info[1]);
        if (!channel)
            ERR_NOSUCHCHANNEL(info[1], kicker->getFd(), _errMsg)
		if (!kicked)
        	ERR_NOSUCHNICK(info[1], kicker->getFd(), _errMsg)
        if (info.size() > 2 && kicked)
            channel->cmdKick(kicked, kicker, args);
		if (!channel->usersOnChannel())
			_channelHandler.rmvChannel(info[1]);
    }
	else
    	ERR_NEEDMOREPARAMS(std::string("KICK"), kicker->getFd(), _errMsg);
}

/**
 * @brief			Parses the part command, check for errors and leaves the channels passed by argument
 * @param args		String vector with the channels to part
 * @param parter	Client issuing the /part command
 */
void	Server::partCmd(const std::vector<std::string> &args, Client *parter) {

	if (args.size() < 2)
		ERR_NEEDMOREPARAMS(std::string("PART"), parter->getFd(), _errMsg)
	
	std::vector<std::string> channels = ft_split(args[1], ',');
	std::vector<std::string>::iterator it;

	for (it = channels.begin(); it != channels.end(); it++) {
		if (Channel *part = _channelHandler.finder((*it))) {
			if (!part->usersOnChannel(parter))
				ERR_NOTONCHANNEL(part->getName(), parter->getFd(), _errMsg)
			part->partChannel(parter);
			if (!part->usersOnChannel())
				_channelHandler.rmvChannel((*it));
		}
		else
			ERR_NOSUCHCHANNEL((*it), parter->getFd(), _errMsg);
	}
}

void	Server::quitCmd(Client *quiter) {
	std::vector<Channel *>::iterator	it;
	std::string							sendMsg;

	if (!quiter->getChannels().empty()) {
		std::vector<Channel *>	channels = quiter->getChannels();

		for (it = channels.begin(); it != channels.end(); it++) {
        	sendMsg = ":" + quiter->getNick() + " QUIT " + (*it)->getName() + "\r\n";
			(*it)->sendMsgToUsers(sendMsg);
			(*it)->rmvClient(quiter);
			if (!(*it)->usersOnChannel()){
				_channelHandler.rmvChannel((*it)->getName());
			}
		}
	}
	destroyClient(quiter);
}

/**
 * @brief			Answer to PING command
 * @param pinger	Client that pings`
 */
void	Server::pong(Client *pinger) 
{
	std::string sendMsg = "PONG :" + pinger->getHost() + "\r\n";
	send(pinger->getFd(), sendMsg.c_str(), sendMsg.length(), 0);
}

/**
 * @brief			Searches the oper list for the client and returns it. Returns NULL if it's not there
 * @param client	Client to search
 */
Client *Server::operFinder(Client *client) {
	std::string	nick = client->getNick();

    for (_it = _opers.begin(); _it != _opers.end(); _it++) {
        if (!(*_it)->getNick().compare(nick))
            return (*_it);
    }
    return (NULL);
}

/**
 * @brief			Searches the oper list for the client and if it doesn't exist adds it
 * @param client	Client to add
 */
void	Server::addOper(Client *client) {
	if (!operFinder(client))
		_opers.push_back(client);
}

/**
 * @brief			Searches the oper list for the client and if it exists removes it
 * @param client	Client to remove
 */
void	Server::delOper(Client *client) {
	if (operFinder(client))
		ERASE_VEC(_opers, _it)
}

/**
 * @brief			Destroy client
 * @param client	Client to destroy
 */
void	Server::destroyClient(Client *client) {
	delOper(client);
	close(client->getFd());
	_sock.setClientSocket(client->getFd() - 4, 0);
	_clientHandler.rmvClient(client->getNick());
}
