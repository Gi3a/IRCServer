# IRCServer
Simple IRC Server on C++

## Commands
### USER <username> <realname> <password>
(The USER message is used at the start of a connection to specify the user name, real name and password of a new user)

### PASS <password>
(The PASS command is used to set the 'password of server')

### NICK <nick>
(The NICK message is used to give a user a nickname)

### MSG <nickname>
MSG #<channel>
(The MSG message is used to send message to user or channel)

### JOIN #<channel>
(The JOIN command is used by a client to start listening to a specific channel)

### OPER <user> <password>
(The OPER message is used by a normal user to obtain the operator privilege)

### QUIT <message>
(A client session ends with a QUIT message can add a leave message)

### KILL <user> <message>
(The KILL message is used to remove a user from the server)

### LUSERS
(The list of users)