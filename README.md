# IRCServer
Simple IRC Server on C++

## Commands
PASS <password>
(The PASS command is used to set the 'login password')

 changed his nick to <pseudonyme>
(The NICK message is used to give a user a nickname)

USER <username> . . <real name>
(The USER message is used at the start of a connection to specify the user name, host name, server name, and real name of a new user)

 joins <channel1,channel2>
(The JOIN command is used by a client to start listening to a specific channel)

OPER <user> <password>
(The OPER message is used by a normal user to obtain the operator privilege)

 quits ([<Quit)
(A client session ends with a QUIT message can add a leave message)

From : to
(PRIVMSG is used to send a private message between users)

OPER (OPER command)
OPER is used to have operator privileges

KILL <user> <comment>
(The KILL command is used to remove a user from the server)