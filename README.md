# ft_IRC

TO DO LIST:
```
[ ] - Main Funtion
    [ ] - Args: Port + Password
    [ ] - Infinite Loop
    [ ] - Socket Creation

[ ] - Client Locator
    [ ] - Nickname & Username
    [ ] - Authenticate
    [ ] - Join Channel

[ ] - Message Relaying
    [ ] - Reading from Server Socket
    [ ] - Parsing Message
    [ ] - Writing to Client Socket

[ ] - Channel Hosting and Management
    [ ] - Create Channel
    [ ] - Delete Channel
    [ ] - Users List Management (Creating and Deleting Users)

[ ] - Classes

    [ ] - Server
            - Interpreter
                - Create User
                - Create Channel
                - Sending Mensages
            - Socket
            - Users <vector>
            - Channels <vector>

    [ ] - User:
            - IP
            - Port
            - Nickname
            - Password
            - Channels ID <vector>

    [ ] - Channel
            - ID    
            - Name

    [ ] - Message
            - Source
            - Dest
            - Content
```

URLS:

Tutorial Install HexChat: https://sourcedigit.com/24996-how-to-install-hexchat-irc-client-on-ubuntu-linux/
What is a Socket? https://www.tutorialspoint.com/unix_sockets/what_is_socket.htm


https://medium.com/the-complete-guide-for-irc-network-i-e-freenode/irc-protocol-services-and-architecture-4e23da2db62


Install WeeChat: sudo apt install weechat


*** ALL INFO IS FROM THE RFC (https://www.rfc-editor.org/rfc/rfc1459.html):

" The IRC protocol is a text-based protocol, with the simplest client
   being any socket program capable of connecting to the server."

The IRC network is like a Spanning tree, where each server is a central node ofr the rest the network it sees.

Client has a unique Nickname (9 characters)!! +  the real name of the host
   that the client is running on, the username of the client on that
   host, and the server to which the client is connected.

Operators are a special type od clients, with privilegies. They can, for example, kill a connection between a client and a server by force. This aims to keep the order in an IRC network.

Channels are groups of 1 or more clients, created when the first joins and ended when the last leave. The message is sent to all clients in the channel.
    Channel name begin with '&' or '#' character and have up to 200 chars. May not contain spaces(' '). control G(^G or ASCII 7), or commas (',').
    If channel do NOT exist, channel is created and the first user becomes a channel operator.

Channel Operators (also referred to as a "chop" or "chanop" and identified by the '@' symbol) have special "powers": 

        KICK    - Eject a client from the channel
        MODE    - Change the channel's mode
        INVITE  - Invite a client to an invite-only channel (mode +i)
        TOPIC   - Change the channel topic in a mode +t channel
