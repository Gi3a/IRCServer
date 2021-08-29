#ifndef SERVER_HPP
#define SERVER_HPP

# include "include.hpp"
# include "Start.hpp"
# include "Channel.hpp"
# include "User.hpp"

class Server
{
private:
	Server();
	Start _start;

	int		create_tcp_server_socket();
	struct pollfd _pollfds[MAX_FDS];

	int	_nb_fds; // count clients fd

	std::map<int, User> _userList;
	std::map<std::string, Channel> _channels;

public:
	Server(Start start);
	Server(Server const & other);
	Server & operator=(Server const & other);
	~Server();

	void	config();
	void	run();
    
    void	send_to_fd(std::string code, std::string message, User const & user, int fd, bool dispRealName) const;
    void	close_fd(int fd);

	int		whichCommand(std::string & request);
	int		checkPassword(User & user);
    int		check_unregistered(int fd);
	int		checkRegistration(int fd);

    void	processRequest(std::string & request, int fd);
	void	parseRequest(std::string request, int fd);
	void	joinMsgChat(User const & user, std::string channel, int fd, std::string command, std::string message);
    
	std::string	getNbUsers() const;
	std::string	getNbChannels() const;
	

	void 	passCommand(std::string & request, int fd);
	void 	nickCommand(std::string & request, int fd);
	void 	userCommand(std::string & request, int fd);
	void 	joinCommand(std::string & request, int fd);
	void 	operCommand(std::string & request, int fd);
	void 	quitCommand(std::string & request, int fd);
	void 	privmsgCommand(std::string & request, int fd);
	void 	lusersCommand(std::string & request, int fd);
	void 	motdCommand(std::string & request, int fd);
	void	helpCommand(std::string & request, int fd);
	void	killCommand(std::string & request, int fd);
};

#endif