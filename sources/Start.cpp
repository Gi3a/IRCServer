#include "../includes/Start.hpp"




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
	return "StartException: Port must contain only numbers";
}

char const * Start::badNetworkException::what( void ) const throw()
{
	return "StartException: Network data format - [host:port_network:password_network]";
}