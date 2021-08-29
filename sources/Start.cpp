#include "../includes/Start.hpp"

Start::Start(int ac, char **av) : _networkPort(0) {
	if (ac < 3 || ac > 4)
		throw Start::badArgumentException();
	int i = ac - 1;
	_password = std::string(av[i--]);
	_port = atoi(av[i]);
	std::string tmp (av[i--]);
	if (_password.empty())
		throw Start::emptyPasswordException();
	if ((_port < 1024) || (_port > 65535) || (tmp.find_first_not_of("0123456789") != std::string::npos) || (tmp.empty()))
		throw Start::badPortException();
	if (i) {
		std::string data(av[i]);
		if (std::count(data.begin(), data.end(), ':') != 2)
			throw Start::badNetworkException();
		unsigned first = data.find_first_of(':');
		unsigned last = data.find_last_of(':');
		_networkHost = data.substr(0, first);
		if (_networkHost.empty())
			throw Start::badNetworkException();
		_networkPort = atoi(data.substr(first + 1, last - first).c_str());
		if (data.substr(first, last - first - 1).empty())
			throw Start::badNetworkException();
		_networkPassword = data.substr(last);
	}
}

Start & Start::operator=(Start const &other) {
	_port = other._port;
	_password = other._password;
	_networkHost = other._networkHost;
	_networkPort = other._networkPort;
	_networkPassword = other._networkPassword;
	return (*this);
}

Start::Start(Start const & other) {
	_port = other._port;
	_password = other._password;
	_networkPort = other._networkPort;
	_networkHost = other._networkHost;
	_networkPassword = other._networkPassword;
}

Start::~Start(){ }

int	Start::getPort() const{
	return _port;
}
int	Start::getNetworkPort() const {
	return _networkPort;
}

std::string const & Start::getPassword() const {
	return _password;
}
std::string const & Start::getNetworkHost() const {
	return _networkHost;
}
std::string const & Start::getNetworkPassword() const {
	return _networkPassword;
}

char const * Start::badArgumentException::what( void ) const throw()
{
	return "StartException: Invalid arguments - [host:port_network:password_network] <port> <password>";
}

char const * Start::emptyPasswordException::what( void ) const throw()
{
	return "StartException: Password is empty";
}

char const * Start::badPortException::what( void ) const throw()
{
	return "StartException: Port must contain only numbers, (ports from 1024 to 65535 are reserved)";
}

char const * Start::badNetworkException::what( void ) const throw()
{
	return "StartException: Network data format - [host:port_network:password_network]";
}

std::ostream & operator<<(std::ostream & o, Start const & s)
{
	o << "\n\n\n" << std::endl;
	o << "IRC start... " << std::endl;
	o << "Port: " << s.getPort() << std::endl;
	o << "Password: " << s.getPassword() << std::endl;
	o << "NetworkPort: " << s.getNetworkPort() << std::endl;
	o << "NetworkHost: " << s.getNetworkHost() << std::endl;
	o << "NetworkPassword: " << s.getNetworkPassword() << std::endl;
	o << "\n\n\n" << std::endl;
	return o;
}