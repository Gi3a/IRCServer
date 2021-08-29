#ifndef HEAD_HPP
#define HEAD_HPP

#include <exception>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <sstream>
#include <map>
#include <cstring>
#include <algorithm>

#define MAX_FDS 1000
#define DATA_BUFFER 10000
#define PWD_OPER "root"
#define SERVER_NAME "ft_irc.com"
#define ALLOWED_CHAR "`|^_-{}[]\\abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
const std::string reset("\033[0m");
const std::string red("\033[0;31m");
const std::string blue("\033[0;34m");
const std::string green("\033[0;32m");
const std::string yellow("\033[0;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");

#endif