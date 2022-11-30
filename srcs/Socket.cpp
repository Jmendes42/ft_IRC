#include "../include/Socket.hpp"

Socket::Socket(int port) {

	_port = port;
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1) {
		MSG("Cant't create socket");
		exit(1);
	}
}

void 		Socket::init_sockets()
{
	int opt = 1;
	//initialise all client_socket[] to 0 so not checked
	for (int i = 0; i < getMaxClients(); i++)
		client_socket[i] = 0;
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
		MSG("setsockopt");
		exit(2);
	}
}

void	Socket::_bind() {

	_hint.sin_family = AF_INET;
	_hint.sin_port = htons(_port);									// Little endian for intel processors (Host To Network Short)
	// INADDR_ANY contains the IP address of the host. For 
    // server code, this will always be the IP address of 
    // the machine on which the server is running.
	_hint.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(_socketFd, (sockaddr *)&_hint, sizeof(_hint)) == -1) {
		MSG("Can't bind int to IP/port");
		exit(2);
	}

	if (listen(_socketFd, SOMAXCONN) == -1) {						// SOMAXCON = max number of communications » 128
		MSG("Can't listen!");
		exit(3);
	}
	_clientSize = sizeof(_hint);
}

void	Socket::sets()
{
	//clear the socket set
	FD_ZERO(&readfds);

	//add master socket to set
	FD_SET(_socketFd, &readfds);
	max_sd = _socketFd;
		
	//add child sockets to set
	for (int i = 0 ; i < getMaxClients() ; i++)
	{
		//socket descriptor
		sd = client_socket[i];
			
		//if valid socket descriptor then add to read list
		if(sd > 0)
			FD_SET( sd , &readfds);
			
		//highest file descriptor number, need it for the select function
		if(sd > max_sd)
			max_sd = sd;
	}
}

void	Socket::activity()
{
	char *message = "001 WELCOME isousa \r\n";
	int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
	if ((activity < 0) && (errno!=EINTR))
	{
		MSG("Select Error");
		exit(2);
	}
		
	//If something happened on the master socket ,
	//then its an incoming connection
	if (FD_ISSET(_socketFd, &readfds))
	{
		if ((new_socket = accept(_socketFd, (struct sockaddr *)&_hint, (socklen_t*)&_clientSize)) < 0)
		{
			MSG("Accepting Error");
			exit(2);
		}
		
		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(_hint.sin_addr) , ntohs(_hint.sin_port));
	
		//send new connection greeting message
		if( send(new_socket, message, strlen(message), 0) != strlen(message) )
		{
			MSG("Sending Message Error");
			exit(2);
		}
		//add new socket to array of sockets
		for (int i = 0; i < getMaxClients(); i++)
		{
			//if position is empty
			if( client_socket[i] == 0 )
			{
				client_socket[i] = new_socket;
				printf("Adding to list of sockets as %d\n" , i);
				break;
			}
		}
	}
	//else its some IO operation on some other socket
	for (int i = 0; i < getMaxClients(); i++)
	{
		sd = client_socket[i];
		if (FD_ISSET( sd , &readfds))
		{
			//Check if it was for closing , and also read the
			//incoming message
			if ((valread = read( sd , buffer, 1024)) == 0)
			{
				//Somebody disconnected , get his details and print
				getpeername(sd , (struct sockaddr*)&_hint , (socklen_t*)&_clientSize);
				printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(_hint.sin_addr) , ntohs(_hint.sin_port));
					
				//Close the socket and mark as 0 in list for reuse
				close( sd );
				client_socket[i] = 0;
			}
			//Echo back the message that came in
			else
			{
				//set the string terminating NULL byte on the end
				//of the data read
				send(sd, ":jmendes!jmendes@localhost 353 jmendes = #tardiz :@jmendes\n"
				":jmendes!jmendes@localhost 366 jmendes #tardiz :End of /NAMES list\n"
				":jmendes!jmendes@localhost JOIN :#tardiz\n", 168, 0);
			}
		}
	}
}


// void	Socket::_clientSet() {

// 	// The listen system call allows the process to listen 
//     // on the socket for connections. 
//     // The program will be stay idle here if there are no 
//     // incomming connections.
// 	if (listen(_socketFd, SOMAXCONN) == -1) {						// SOMAXCON = max number of communications » 128
// 		MSG("Can't listen!");
// 		exit(3);
// 	}
// 	_clientSize = sizeof(_client);
// 	_clientSocket = accept(_socketFd, (sockaddr *)&_client, &_clientSize);
// 	if (_clientSocket == -1) {
// 		MSG("Problem with client connecting!");
// 		exit(4);
// 	}
// 	close(_socketFd);
// 	ft_memset(_host, 0, NI_MAXHOST);
// 	ft_memset(_svc, 0, NI_MAXSERV);
// }
