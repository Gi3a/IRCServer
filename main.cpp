#include "includes/include.hpp"

int main(int ac, char **av)
{
    if (ac == 4)
        std::cout << "ircserv is start..." << std::endl;
    else
        std::cout << "params: [host:port_network:password_network] <port> <password>" << std::endl;
    return (0);
}