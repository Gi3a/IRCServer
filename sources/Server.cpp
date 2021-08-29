#include "../includes/Server.hpp"

Server::Server(Start start) : _start(start), _nb_fds(0){

}

Server::~Server() {
	std::map<int, User>::iterator it;

	for (it = _userList.begin(); it != _userList.end(); it++)
	{
		close(it->first);
	}
	_userList.clear();
	close(_pollfds[0].fd);
}

void	Server::config() {
	_pollfds[0].fd = Server::create_tcp_server_socket();
	_pollfds[0].events = POLLIN;
	_pollfds[0].revents = 0;
	_nb_fds = 1;
}

int		Server::create_tcp_server_socket() {
	struct sockaddr_in saddr;
	int fd, ret_val;

	/*Create tcp socket*/
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if (fd == -1)
		throw std::runtime_error("Creating server socket failed\n");
	std::cout << "Server socked created with fd [" << fd << "]" << std::endl;
	fcntl(fd, F_SETFL, O_NONBLOCK);

	/*init socket address structure + bind*/
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(_start.getPort());
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
	if (ret_val != 0) {
		close(fd);
		throw std::runtime_error("Binding failed, socket has been closed\n");
	}

	/*listen for incoming connections*/
	ret_val = listen(fd, 10);
	if (ret_val != 0) {
		close(fd);
		throw std::runtime_error("Listen failed, socket has been closed\n");
	}
	return fd;
}

void	Server::run() {
	socklen_t addrlen = sizeof(struct sockaddr_storage);
	struct sockaddr_storage client_saddr;
	int ret_val;
	char buf[DATA_BUFFER];

	while (1) {
		std::cout << red << "\nCurrently listening to " << _nb_fds - 1 << " clients" << reset << std::endl;
		if (poll(_pollfds, _nb_fds, -1) == -1)
			throw std::runtime_error("Error during poll\n");
		for (int fd = 0; fd < (_nb_fds); fd++) {
			if ((_pollfds + fd)->fd  <= 0)
				continue;
			if (((_pollfds + fd)->revents & POLLIN) == POLLIN){
				if (fd == 0) { //if event occured on the socket server
					int new_fd;
					if ((new_fd = accept(_pollfds[0].fd, (struct sockaddr *) &client_saddr, &addrlen)) == -1)
						std::cerr << "Accept failed" << std::endl;
					std::cout << "New connection accepted on fd[" << green << new_fd << reset << "]" << std::endl;
					_userList.insert(std::pair<int, User>(new_fd, User()));
					(_pollfds + _nb_fds)->fd = new_fd;
					(_pollfds + _nb_fds)->events = POLLIN;
					(_pollfds + _nb_fds)->revents = 0;
					if (_nb_fds < MAX_FDS)
						_nb_fds++;
				}
				else {
					ret_val = recv((_pollfds + fd)->fd, buf, DATA_BUFFER, 0);
					if (ret_val == -1)
						std::cerr << "Recv failed" << std::endl;
					else if (ret_val == 0) {
						std::cout << "Connection on fd[" << green << (_pollfds + fd)->fd << reset << "] closed by client" << std::endl;
						std::string rep ("QUIT");
						quitCommand(rep, (_pollfds + fd)->fd);
					}
					else {
						std::string data(buf);
						parseRequest(data, (_pollfds + fd)->fd);
						memset(buf, 0, DATA_BUFFER);
					}
				}
			}
		}
	}
}

void	Server::processRequest(std::string & request, int fd) {
	while(request.size() && isspace(request.front())) request.erase(request.begin()); // removes first spaces
	while(request.size() && isspace(request.back())) request.pop_back(); //remove last spaces
	if (whichCommand(request) > -1) {
		void		(Server::*ptr[])(std::string &, int) = {
		&Server::passCommand,
		&Server::nickCommand,
		&Server::userCommand,
		&Server::joinCommand,
		&Server::operCommand,
		&Server::quitCommand,
		&Server::msgCommand,
		&Server::lusersCommand,
		&Server::helpCommand,
		&Server::killCommand
		};
		(this->*ptr[whichCommand(request)]) (request, fd);
	}
	else {
		std::istringstream iss(request);
		std::string command;
		iss >> command;
		send_to_fd("421", std::string(command) +" :Unknown command", _userList[fd], fd, false);
	}
}

int	Server::whichCommand(std::string & request) {
	const char* arr[] = {"PASS","NICK","USER","JOIN","OPER","QUIT","MSG","LUSERS", "HELP", "KILL"};
	std::istringstream iss(request);
	std::string firstWord;
	std::vector<std::string>::iterator it;

	iss >> firstWord;
	std::transform(firstWord.begin(), firstWord.end(),firstWord.begin(), ::toupper);
	std::vector<std::string> commandList(arr, arr + sizeof(arr)/sizeof(arr[0]));
	if (find(commandList.begin(), commandList.end(), firstWord) != commandList.end())
		for (size_t i = 0; i < commandList.size(); i++)
			if (firstWord == commandList[i])
				return i;
	return -1;
}

void Server::passCommand(std::string & request, int fd) {
	std::string str = request.substr(strlen("PASS"));

	if (str.empty()){
		send_to_fd("461", "PASS :Syntax error", _userList[fd], fd, false);
		return;
	}
	str = str.substr(str.find_first_not_of(" "));
	if (std::count(str.begin(), str.end(), ' ') > 0 && str[0] != ':') {//there is more than one word, not rfc compliant
		send_to_fd("461", "PASS :Syntnax error", _userList[fd], fd, false);
		return;
	}
	str.erase( std::remove(str.begin(), str.end(), '\n'), str.end() );
	if (_userList[fd].getNickname().compare("*") != 0 || !_userList[fd].getUsername().empty()) // already registered if nickname or username not empty
	{
		send_to_fd("462", ":Connection already registered", _userList[fd], fd, false);
		return;
	}
	_userList[fd].setTmpPwd(str);
	std::cout << "Password saved" << std::endl;
}

void Server::nickCommand(std::string & request, int fd) {
	std::string str = request.substr(strlen("NICK"));
	std::stringstream 	stream(str);
	std::string		firstArg,secondArg;
	std::string		oldNick;
	unsigned int	countParams = 0;
	if(stream >> firstArg) { ++countParams;}
	if(stream >>secondArg) { ++countParams;}
	if(stream>>secondArg) { ++countParams;}

	if (countParams < 1 || countParams > 2 || (firstArg.length() > 9) || firstArg.find_first_not_of(ALLOWED_CHAR) != std::string::npos){
		send_to_fd("461", "NICK :Syntax error", _userList[fd], fd, false);
		return;
	}
	for (std::map<int, User>::iterator it = _userList.begin(); it != _userList.end(); it++)
		if (it->second.getNickname().compare(firstArg) == 0) { // already same nickname
			send_to_fd("462", " :Nickname is already in use", it->second, fd, false);
			return;
		}
	if (_userList[fd].getNickname().compare("*") != 0)
		oldNick = _userList[fd].getNickname();
	_userList[fd].setNickname(firstArg);
	if (_userList[fd].isRegistered())
	{
		std::string rep(":"); rep += oldNick; rep += "!~"; rep += _userList[fd].getUsername(); rep += "@localhost NICK :"; rep += firstArg; rep += "\n";
		send(fd, rep.c_str(), rep.length(), 0);
		return ;
	}
	std::cout << "Nickname saved" << std::endl;
	if (_userList[fd].getNickname().compare("*") != 0 && !(_userList[fd].getUsername().empty())) {
		if (!checkRegistration(fd))
		{
			std::string rep("ERROR :Access denied: Bad password? (nick command)\n");
			send(fd, rep.c_str(), rep.length(), 0);
			std::cout << rep;
			close_fd(fd);
			return;
		}
		else
			_userList[fd].setRegistered(1);
	}
}

void Server::userCommand(std::string & request, int fd) {
	std::string str = request.substr(strlen("USER"));
	std::stringstream 	stream(str);
	std::string		oneWord, firstword;;
	unsigned int	countParams = 0;
	if (stream >> firstword) ++countParams;
	while(stream >> oneWord) { ++countParams;}

	/*bad user syntax, doesnt respect  <username> * * <realname> */
	if (countParams != 4){
		send_to_fd("461", "USER :Syntax error", _userList[fd], fd, false);
		return;
	}
	if (_userList[fd].getNickname().compare("*") != 0 && !_userList[fd].getRealName().empty())
	{
		send_to_fd("462", ":Connection already registered", _userList[fd], fd, false);
		return;
	}
	_userList[fd].setUsername(firstword);
	_userList[fd].setRealname(oneWord.substr(1));
	std::cout << "Username saved" << std::endl;
	/*everything fine, answer to the client*/
	if (_userList[fd].getNickname().compare("*") != 0 && !(_userList[fd].getUsername().empty())) {
		if (!checkRegistration(fd))
		{
			std::string rep("ERROR :Access denied: Bad password? (user command)\n");
			send(fd, rep.c_str(), rep.length(), 0);
			std::cout << rep;
			close_fd(fd);
			return;
		}
		else
			_userList[fd].setRegistered(1);
	}
}

void Server::joinCommand(std::string & request, int fd) {
	if (check_unregistered(fd)) return;
	std::string str = request.substr(strlen("JOIN"));
	std::stringstream 	stream(str);
	std::string		chans;
	std::string		keys;
	unsigned int	countParams = 0;
	if(stream >> chans) { ++countParams;}
	if(stream >> keys) { ++countParams;}
	if(stream >> chans) { ++countParams;}

	std::string firstchan, firstkey;

	if (countParams < 1 || countParams > 2) { //BAD SYNTAX
		send_to_fd("461", "JOIN :Syntax error",_userList[fd], fd, false);
	}
	while (!chans.empty()) { //can have more chan than keys, so we dont care
		if (chans.find(',')) //more than 1 chan
			firstchan = chans.substr(0, chans.find(','));
		else
			firstchan = chans;

		if (keys.find(',')) //more than 1 keys
			firstkey = keys.substr(0, chans.find(',') - 1);
		else
			firstkey = keys;
		std::map<std::string, Channel >::iterator itchan = _channels.find(firstchan);
		if (itchan != _channels.end()) { //add to existing chan
			std::vector<int> users = itchan->second.getUsers(); //check if user isnt already in
			if (find(users.begin(), users.end(), fd) ==  users.end()){
				itchan->second.addUser(fd);
				joinMsgChat(_userList[fd], firstchan, fd, "JOIN", std::string(""));
				std::cout << "Add to existing chan " << blue << firstchan << reset << std::endl;
				for (std::vector<int>::iterator it = users.begin(); it != users.end(); it++)
					if ((*it) != fd)
						joinMsgChat(_userList[fd], firstchan, (*it), "JOIN", std::string(""));
			}
		}
		else { //chan must begin with & or #, cant contain spaces/ctrl G/comma
			if ((firstchan.find_first_of("&#") == 0) && (firstchan.find_first_of(" ,\x07") == std::string::npos)) {
				_channels.insert(std::pair<std::string, Channel>(firstchan, Channel(fd, firstchan, firstkey)));
				joinMsgChat(_userList[fd], firstchan, fd, "JOIN", std::string(""));
				std::cout << "Creating new chan : " << blue << firstchan << reset << std::endl;
			}
			else //bad chan name
				send_to_fd("403", std::string(firstchan)+" :No such channel", _userList[fd], fd, false);
		}
		if (chans.find(',') != std::string::npos) //more than 1 chan
			chans = chans.substr(chans.find(',')+1);
		else
			chans.erase();
	}
}

void Server::operCommand(std::string & request, int fd) {
	if (check_unregistered(fd)) return;
	std::string str = request.substr(strlen("OPER"));
	std::stringstream 	stream(str);
	std::string		user;
	std::string		password;
	unsigned int	countParams = 0;
	if (stream >> user) { ++countParams;}
	if (stream >> password) { ++countParams;}
	if (stream >> password) { ++countParams;}

	if (countParams != 2) {
		send_to_fd("461", "OPER :Syntax error", _userList[fd], fd, false);
		return;
	}
	if (password.compare(PWD_OPER) == 0) {
		_userList[fd].setOperName(user);
		send_to_fd("381", ":You are now an IRC operator", _userList[fd], fd, false);
	}
	else {
		send_to_fd("464", ":Password incorrect", _userList[fd], fd, false);
	}
}

void Server::quitCommand(std::string & request, int fd) {
	std::vector<int> users_to_contact;

	std::string str = request.substr(strlen("QUIT"));
	std::string message;

	if (str.empty()) //no message, init with nickname
		message = ":Client closed connection";
	else
		message = str.substr(str.find_first_not_of(" "));
	if (std::count(message.begin(), message.end(), ' ') > 0 && message[0] != ':') {//there is more than one word, : needed
		send_to_fd("461", "QUIT :Syntax error", _userList[fd], fd, false);
		return;
	}
	std::map<std::string, Channel >::iterator ite = _channels.end();

	for (std::map<std::string, Channel >::iterator it = _channels.begin(); it != ite; it++) //fill users to contact with users who shares channels with the leaver
	{
		std::vector<int> tmp_users = it->second.getUsers();
		if (find(tmp_users.begin(), tmp_users.end(), fd) != tmp_users.end())
		{
			it->second.eraseUser(fd);
			for (std::vector<int>::iterator itusers = tmp_users.begin(); itusers != tmp_users.end(); itusers++)
			{
				if (find(users_to_contact.begin(), users_to_contact.end(),(*itusers)) == users_to_contact.end())
				users_to_contact.push_back((*itusers));
			}
		}
	}
	for (std::vector<int>::iterator contact = users_to_contact.begin(); contact != users_to_contact.end(); contact++)
	{
		if (*contact != fd) {
			std::string rep(":");
			rep += _userList[fd].getNickname();
			rep += "!~";
			rep += _userList[fd].getUsername();
			rep += "@localhost QUIT ";
			rep += message;
			rep += "\n";
			send((*contact), rep.c_str(), rep.length(), 0);
			std::cout << rep;
		}
	}
	close_fd(fd);
}

void Server::msgCommand(std::string & request, int fd) {
	if (check_unregistered(fd)) return;
	std::string str = request.substr(strlen("MSG"));
	std::stringstream 	stream(str);
	std::string		dests, message, tmp, firstdest;
	unsigned int	countParams = 0;
	if(stream >> dests) { ++countParams;}
	if(stream >> message) { ++countParams;}
	while (stream >> tmp) { ++countParams;}

	if (countParams == 0) {
		send_to_fd("411", ":No recipient given (MSG)", _userList[fd], fd, false);
		return;
	}

	if (countParams == 1) {
		send_to_fd("412", "No text to send", _userList[fd], fd, false); //only dest, no params
		return;
	}

	if (countParams > 2 && message[0] != ':') {//there is more than one word, : needed
		send_to_fd("461", "MSG :Syntnax error", _userList[fd], fd, false);
		return;
	}
	str = str.substr(str.find(dests) + dests.length());
	str = str.substr(str.find_first_not_of(" "));
	if (str[0] == ':') str = str.substr(1); //remove first char if ":"

	while(!dests.empty()) {
		bool disp = true;
		if (dests.find(',')) //more than 1 chan
			firstdest = dests.substr(0, dests.find(','));
		else
			firstdest = dests;
		std::map<std::string, Channel >::iterator itchan = _channels.find(firstdest);
		if (itchan != _channels.end()) {
			std::vector<int> users = itchan->second.getUsers();
			for (std::vector<int>::iterator it = users.begin(); it != users.end(); it++) {
				if ((*it) != fd) {
					joinMsgChat(_userList[fd], firstdest, (*it), "MSG", str);
				}
				disp = false;
			}
		}
		for (std::map<int, User>::iterator it = _userList.begin(); it != _userList.end(); it++)
		{
			if (it->second.getNickname() == firstdest)
			{
				joinMsgChat(_userList[fd], firstdest, it->first, "MSG", str);
				disp = false;
			}
		}
		if (disp)
			send_to_fd("401", ":No such nick or channel name",_userList[fd],fd,false);
		if (dests.find(',') != std::string::npos) //more than 1 chan
			dests = dests.substr(dests.find(',')+1);
		else
			dests.erase();
	}
}

void Server::lusersCommand(std::string & request, int fd) {
	if (check_unregistered(fd)) return;
	(void)request;
	send_to_fd("251",std::string(":There are ")+ getNbUsers() + " users, 0 servuces and 1 servers", _userList[fd], fd, false);
	send_to_fd("254",std::string(getNbChannels()) + " :channels formed", _userList[fd], fd, false);
	send_to_fd("255",std::string(":I have ")+ getNbUsers() + " users, 0 service and 0 servers", _userList[fd], fd, false);
}

void Server::helpCommand(std::string & request, int fd) {
	(void)request;
	std::string rep("The recommended order of orders for registering a customer is as follows:\n");
	rep += "PASS <password>\n";
	rep += "(The PASS command is used to set the 'login password')\n\n";
	rep += "NICK <pseudonyme>\n";
	rep += "(The NICK message is used to give a user a nickname)\n\n";
	rep += "USER <username> . . <real name>\n";
	rep += "(The USER message is used at the start of a connection to specify the user name, host name, server name, and real name of a new user)\n\n";
	rep += "JOIN <channel1,channel2> <key1,key2>\n";
	rep += "(The JOIN command is used by a client to start listening to a specific channel)\n\n";
	rep += "OPER <user> <password>\n";
	rep += "(The OPER message is used by a normal user to obtain the operator privilege)\n\n";
	rep += "QUIT [<Quit message>]\n";
	rep += "(A client session ends with a QUIT message can add a leave message)\n\n";
	rep += "MSG <recipient>(1 or more) <:text to send>\n";
	rep += "(MSG is used to send a private message between users)\n\n";
	rep += "OPER (OPER command)\n";
	rep += "OPER is used to have operator privileges\n\n";
	rep += "KILL <user> <comment>\n";
	rep += "(The KILL command is used to remove a user from the server)\n\n";
	send(fd, rep.c_str(), rep.length(), 0);
}

void	Server::killCommand(std::string & request, int fd) {
	if (check_unregistered(fd)) return;
	std::string str = request.substr(strlen("KILL"));
	std::stringstream 	stream(str);
	std::string		target, message, tmp;
	unsigned int	countParams = 0;
	if (stream >> target) { ++countParams;}
	if (stream >> message) { ++countParams;}
	while (stream >> tmp) {++countParams;}

	if (countParams < 2 || (countParams > 2 && message[0] != ':')) {
		send_to_fd("461", "KILL :Syntax error", _userList[fd], fd, false);
		return;
	}
	if (_userList[fd].getOperName().empty()) {
		send_to_fd("481", ":Permission Denied- You're not an IRC operator",_userList[fd],fd,false);
		return;
	}
	for (std::map<int, User>::iterator it = _userList.begin(); it != _userList.end(); it++){
		if (target == it->second.getNickname()) {
			std::string rep("ERROR : KILLed by ");
			rep += _userList[fd].getNickname();
			rep += ": ";
			rep += str;
			rep += "\n";
			send(it->first, rep.c_str(), rep.length(), 0);
			close_fd(it->first);
			return;
		}
	}
}

int	Server::checkPassword(User & user){
	if (user.getTmpPwd() != _start.getPassword())
		return false;
	return true;
}

void	Server::parseRequest(std::string request, int fd){
	std::string parse;
	std::map<int, User>::iterator it = _userList.find(fd);

	request.erase(std::remove(request.begin(), request.end(), '\r'), request.end()); //erase \r, to work with netcat or irc client
	while (!request.empty())
	{
		if (request.find('\n') == std::string::npos) {// no \n found, incomplete request, add to user
			_userList[fd].appendTmpRequest(request);
			break;
		}
		else { //\n found, but maybe more than 1, check User._tmpRequest to append with it
			parse = it->second.getTmpRequest().append(request.substr(0, request.find_first_of("\n")));
			_userList[fd].cleanTmpRequest(); //request is complete, we can clean tmpReq;
			processRequest(parse, fd);
		}
		request = request.substr(request.find_first_of("\n") + 1);
	}
}

std::string	Server::getNbUsers() const{
	std::stringstream ss;
	ss << _userList.size();
	return ss.str();
}

std::string	Server::getNbChannels() const{
	std::stringstream ss;
	ss << _channels.size();
	return ss.str();
}

void	Server::send_to_fd(std::string code, std::string message,
User const & user, int fd, bool dispRealName) const {
	std::string rep(":");
	rep += SERVER_NAME;
	rep += " ";
	rep += code;
	rep += " ";
	rep += user.getNickname();
	rep += " ";
	rep += message;
	if (dispRealName){
		rep += " ";
		rep += user.getNickname();
		rep += "!~";
		rep += user.getUsername();
		rep += "@localhost(sendto)";
	}
	rep += "\n";
	send(fd, rep.c_str(), rep.length(), 0);
	if (code.compare("001") != 0 &&	code.compare("251") != 0 &&
		code.compare("254") != 0 && code.compare("255") != 0)
	std::cout << rep;
	return;
}

void	Server::joinMsgChat(User const & user, std::string channel, int fd, std::string command, std::string message) {
	std::string rep(":");
	rep += user.getNickname();
	rep += "!~";
	rep += user.getUsername();
	rep += "@localhost ";
	rep += command;
	if (command.compare("MSG") == 0)
		rep += (std::string(" ") + channel + " " + message);
	else
		rep += (" :" + channel);
	rep += "\n";
	send(fd, rep.c_str(), rep.length(), 0);
	std::cout << rep;
}

int		Server::checkRegistration(int fd) {
	if (!checkPassword(_userList[fd]))
		return 0;
	std::cout << "User " << _userList[fd].getNickname() << " registered !" << std::endl;
	send_to_fd("001", ":Welcome to our FT_IRC project !", _userList[fd], fd, false);
	send_to_fd("251",std::string(":There are ")+ getNbUsers() + " users, 0 services and 1 servers", _userList[fd], fd, false);
	send_to_fd("254",std::string(getNbChannels()) + " :channels formed", _userList[fd], fd, false);
	send_to_fd("255",std::string(":I have ")+ getNbUsers() + " users, 0 services and 0 servers", _userList[fd], fd, false);
	return 1;
}

int	Server::check_unregistered(int fd){
	// if (_userList[fd].getUsername().empty() || (_userList[fd].getNickname().compare("*") == 0)) {
	if (_userList[fd].getUsername().empty()) {
		std::cout << "USER unreg: " << _userList[fd].getUsername() << std::endl;
		// std::string rep(":");
		// rep += SERVER_NAME;
		// rep += " 451 ";
		// rep += _userList[fd].getNickname();
		// rep += " :Connection not registered (unregistered)\n";
		// send(fd, rep.c_str(), rep.length(), 0);
		// std::cout << rep;
		// return 1;
	}
	return 0;
}

void	Server::close_fd(int fd){
	for (int i = 1; i < _nb_fds; i++){
		if (_pollfds[i].fd == fd) { //find which pollfds correspond to client's fd
			_pollfds[i].fd *= -1;
			if (i == _nb_fds - 1) // last poll, just close and decr fd number
				close(fd);
			else { //switch the one to delete with the last one
				_pollfds[i] = _pollfds[_nb_fds - 1];
				close(fd);
			}
			_userList.erase(fd);
			_nb_fds--;
			}
		}
}