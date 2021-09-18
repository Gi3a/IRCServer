#ifndef BOT_HPP
#define BOT_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>

// Max size bytes to get for one time
#define MAXDATASIZE 100

# include "include.hpp"

class Bot
{
private:
    std::string name;

public:
	~Bot();
	Bot();

	void botResponse(std::string str, std::string &response);

    std::string const & getName() const;
};

#endif