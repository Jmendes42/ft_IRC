#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostreams>
#include <string.h>

#include <netinet/in.h>

class Socket {


	public:
		inputStream getInputStream();	// Get input from user
										// This input stream method will return the 
										// InputStream representing the data attached to 
										// this socket. It also throws an exception.

		outputStream getOutputStream();	// Get output from user
										// This output stream method will return the 
										// OutputStream representing the data attached to 
										// this socket. It also throws an exception.
		// ^ Make sure the object must be returned every time you call the 
		// above methods to avoid errors.

		synchronized   
};


int	main() {


	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return 0;
}
