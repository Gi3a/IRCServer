#include "includes/Start.hpp"

int main(int ac, char **av)
{
    try
	{
		Start ircArgs(ac, av);
		std::cout << ircArgs << std::endl;
		// ircServer serv(ircArgs);
		// serv.config();
		// serv.run();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
    return (0);
}