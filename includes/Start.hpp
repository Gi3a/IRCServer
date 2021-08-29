#ifndef START_HPP
#define START_HPP

# include "include.hpp"

class Start
{
private:
	Start();
	int         	_port;
	int        		_networkPort;
	std::string 	_password;
	std::string 	_networkPort;
	std::string		_networkPassword;
public:
	Start(int ac, char **av);
	Start(Start const &other);
	Start 			& operator=(Start const & other);
	~Start();

	std::string		& getPassword();
	std::string		& getNetworkHost();
	std::string		& getNetworkPassword();

	int				getPort();
	int				getNetworkHost();
};

#endif