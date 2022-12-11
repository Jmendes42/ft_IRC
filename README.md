# FT_IRC
<br>

## Guidelines

> The IRC protocol is a text-based protocol, with the simplest client being any socket program capable of connecting to the server.

> The IRC network is like a Spanning tree, where each server is a central node ofr the rest the network it sees.
<br>

- ***Client***

> A client is anything connecting to a server that is not another server.
 
        * Nickname (Unique & MAX 9 Chars);
        * Username;
        * Hostname;
    

- ***Channel***
    
> A channel is a named group of one or more clients which will all receive messages addressed to that channel.
    
        * First Client to join = Channel Operator = Creates the Channel;
        * Channel Endes when the last Client leaves;
        * Messages are sent to all Clients in the Channel;
        * Channel's Name begin with '&' or '#' and MAX of 200 chars.
        * Channel's Name may NOT contain spaces(' '). control G(^G or ASCII 7), or commas (',');

- ***Operators***

> Operators are a special type od clients, with more privilegies. This aims to keep the order in an IRC network.

Channel Operators (also referred to as a ***chop*** or ***chanop*** and identified by the '@' symbol) have special **COMMANDS**: 

        KICK    - Eject a client from the channel
        MODE    - Change the channel's mode

                > About -p and -s : "Both can’t be set together. And uppon setting one mode, the other one is automatically unset." 
        
        INVITE  - Invite a client to an invite-only channel (mode +i)
        TOPIC   - Change the channel topic in a mode +t channel

- ***Messages***
    
        * Comunication is essentially asynchronous (meaning, Clients should not wait for a Reply from the Server.)
        * Messages are always terminated with Carriage Return - Line Feed and they should NOT exceed 512 chars.
        * Server must parse the entire message and return appropriate errors.
        * All Parameters must be check for validity.
    
    Divided in 3 Main Parts:
    
    1. Prefix (Opcional)
    
        * Indicated using a colon (':') (ex: ':isousa') *No whitespaces between the ':' and the prefix.
        * Used to indicate the origin of the message.
        * If there is no prefix, Server will assume it is originated from the connection it was received.
        * If the source can't be found (ex: prefix dont match any user) or if the source is different than the link it arrived from, the message should be ignored.
    
    2. Command
    
        * Valid IRC command or a 3 digit number.
    
    3. Command Parameters (MAX 15)

<br>
<br>

## URLS

[RFC](www.rfc-editor.org/rfc/rfc1459.html)

[Socket class](https://www.educba.com/socket-programming-in-c-plus-plus/)

[socket()](https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-socket-create-socket)

[What is a Socket?](https://www.tutorialspoint.com/unix_sockets/what_is_socket.htm)

[IRC Guide](https://medium.com/the-complete-guide-for-irc-network-i-e-freenode/irc-protocol-services-and-architecture-4e23da2db62)

[IRC RFC](https://www.rfc-editor.org/rfc/rfc1459.html)

[IRC Message Examples](http://chi.cs.uchicago.edu/chirc/irc_examples.html)

[POLL()](https://www.ibm.com/docs/ja/i/7.3?topic=ssw_ibm_i_73/apis/poll.html)

[SELECT()](https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/)

[MORE SELECT()](https://www.scottklement.com/rpg/socktut/proxy.html)

[Difference between private and secret channel](https://ircnet.barfooze.de/articles/secretmode/)

[OPER GUIDE]()

[OPER block](https://www.unrealircd.org/docs/Oper_block)

#### NICK

     /nick <nick>                                - set nick on current server
     /set irc.server_default.nicks <nick>        - set default nick dor every server
     /set irc.server.[SERVER_NAME].nicks <nick>  - set default nick for target server
     
    
Install WeeChat: 
```sudo apt install weechat```
