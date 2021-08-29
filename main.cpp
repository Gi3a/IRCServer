#include "includes/Server.hpp"

int main(int ac, char **av)
{
    try
	{
		Start irc(ac, av);
		std::cout << irc << std::endl;
		Server serv(irc);
		serv.config();
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
    return (0);
}