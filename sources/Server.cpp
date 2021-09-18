#include "../includes/include.hpp"
#include "../includes/Server.hpp"
#include "../includes/Bot.hpp"

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
	std::cout << green <<  "Server socked created with fd [" << fd << "]" << reset << std::endl;
	fcntl(fd, F_SETFL, O_NONBLOCK);

	/*init socket address structure + bind*/
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(_start.getPort());
	if (!(_start.getNetworkHost().empty()))
		saddr.sin_addr.s_addr = inet_addr(_start.getNetworkHost().c_str());
	else
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
		std::cout << red << "\nNow: " << _nb_fds - 1 << " clients" << reset << std::endl;
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
	Bot *bot = new Bot();
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
		&Server::killCommand,
		&Server::uploadCommand,
        &Server::downloadCommand
		};
		(this->*ptr[whichCommand(request)]) (request, fd);
	}
	else if (DEBUG)
	{
		std::string firstdest;
		std::string response;

		firstdest = "";
		std::cout << request << std::cout;
		if (request.find("bot1"))
		{
			std::cout << "BOOT" << std::endl;
			bot->botResponse(request, response);
			send_to_fd("421", reset + response, _userList[fd], fd, false);
		}
		else
		{
			std::cout << "NOT BOOT" << std::endl;
			for (std::map<int, User>::iterator it = _userList.begin(); it != _userList.end(); it++) {
				// if (target == it->second.getNickname()) {
				// 	std::string rep("ERROR : KILLed by ");
				// 	rep += _userList[fd].getNickname();
				// 	rep += ": ";
				// 	rep += str;
				// 	rep += "\n";
				// 	send(it->first, rep.c_str(), rep.length(), 0);
				// 	close_fd(it->first);
				// 	return;
				// }
				if (it->first != fd)
					joinMsgChat(_userList[fd], firstdest, it->first, "MSG", cyan + request + reset);
				// send_to_fd("421", cyan + request, _userList[fd], fd, false);
				// send_to_fd("421", cyan + request, _userList[fd], fd, false);
			}
		}
		std::cout << "[" << fd << "] -> " <<  request << reset << std::endl;
	}
	else {
		std::istringstream iss(request);
		std::string command;
		iss >> command;
		send_to_fd("421", reset + std::string(command) +" :Unknown command, use HELP", _userList[fd], fd, false);
	}
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
	std::string rep(reset);
	rep += "\n✧";
	rep += SERVER_NAME;
	rep += " : [user: ";
	rep += user.getNickname();
	rep += "] ";
	rep += message;
	rep += reset;
	if (dispRealName){
		rep += " ";
		rep += user.getNickname();
		rep += "!~";
		rep += user.getUsername();
		rep += "@localhost(sendto)";
	}
	rep += "\n\n\n";
	send(fd, rep.c_str(), rep.length(), 0);
	if (code.compare("001") != 0 &&	code.compare("251") != 0 &&
		code.compare("254") != 0 && code.compare("255") != 0)
	std::cout << rep;
	return;
}

void	Server::joinMsgChat(User const & user, std::string channel, int fd, std::string command, std::string message) {
	std::string rep(reset);
	rep += "\n✧[user: ";
	rep += user.getNickname();
	rep += "] ->";
	if (command.compare("MSG") == 0)
		rep += (std::string("[") + channel + "]: " + message);
	else
		rep += (" " + channel);
	rep += "\n▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n\n";
	send(fd, rep.c_str(), rep.length(), 0);
	std::cout << rep;
}

int	Server::checkPassword(User & user){
	if (user.getTmpPwd() != _start.getPassword())
		return false;
	return true;
}

int		Server::checkRegistration(int fd, int flag) {
	if (_userList[fd].getNickname().length() < 1 || _userList[fd].getUsername().length() < 1 || _userList[fd].getTmpPwd().length() < 1)
	{
		send_to_fd("481", red + ":Permission Denied - Need registration. Try USER <username> <realname> <password>", _userList[fd], fd, false);
		return 0;
	}
	if (!checkPassword(_userList[fd]))
	{
		send_to_fd("481", red + ":Permission Denied - Bad password. Try again PASS", _userList[fd], fd, false);
		return 0;
	}
	if (flag)
		return 1;
	_userList[fd].setRegistered(1);
	std::cout << green + "User " << _userList[fd].getNickname() << " registered !" << std::endl;
	send_to_fd("001", green + ":Welcome to IRCServer!\n" + yellow + "Users : " + getNbUsers() + "\n" + yellow + "Channels : " + getNbChannels() + "\n", _userList[fd], fd, false);
	return 1;
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