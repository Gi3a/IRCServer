#ifndef START_HPP
#define START_HPP

# include "include.hpp"

class Start
{
private:
	Start();
	int					_port;
	int        			_networkPort;
	std::string			_password;
	std::string			_networkHost;
	std::string			_networkPassword;
public:
	Start(int ac, char **av);
	Start(Start const &other);
	Start				& operator=(Start const & other);
	~Start();

	std::string const	& getPassword() const;
	std::string const	& getNetworkHost() const;
	std::string const	& getNetworkPassword() const;

	int				getPort() const;
	int				getNetworkPort() const;

	class badArgumentException : public std::exception
	{
		virtual const char * what() const throw();
	};
	class badPortException : public std::exception
	{
		virtual const char * what() const throw();
	};
	class badNetworkException : public std::exception
	{
		virtual const char * what() const throw();
	};
	class emptyPasswordException : public std::exception
	{
		virtual const char * what() const throw();
	};
};

std::ostream			& operator<<(std::ostream &out, Start const &s);

#endif