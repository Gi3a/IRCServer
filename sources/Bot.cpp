#include "../includes/Bot.hpp"

Bot::Bot()
{
    this->name = "Herman Gref";
}

Bot::~Bot()
{
}

std::string const & Bot::getName() const
{
	return (this->name);
}

void Bot::botResponse(std::string str, std::string &response)
{
	std::string arrGreet[] = {
		"hi",
		"hello",
		"privet",
		"zdarova",
		"hey"
	};

	std::string arrCita[] = {
		"i dont know",
		"kogda mne bilo rovno 5",
		"world peace",
		"people, help the peolpe",
		"buy apricots"
	};

	std::string arrName[] = 
	{
		"broooo",
		"dude",
		"stranger",
		"human",
		"sweaty",
		"honey"
	};
	
	

	if (str.find("hi") != std::string::npos)
	{
		response = arrGreet[rand() % arrGreet->length() - 1] + ", " + arrName[rand() % arrName->length() - 1];
	}
	else
	{
		response = arrCita[rand() % arrCita->length() - 1] + ", " + arrName[rand() % arrName->length() - 1];
	}
	response += "\r\n";
}

// int main(int argc, char *argv[])
// {
// 	int sockfd, numbytes;
// 	char buf[MAXDATASIZE];
// 	struct addrinfo hints, *servinfo, *p;
// 	int rv;
// 	char s[INET6_ADDRSTRLEN];

// 	if (argc != 4) {
// 		printf("bot: <hostname> <port> <password>");
// 		exit(1);
// 	}
// 	memset(&hints, 0, sizeof hints);
// 	hints.ai_family = AF_UNSPEC; // IPv4  IPv6
// 	hints.ai_socktype = SOCK_STREAM; // TCP stream-sockets

// 	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
// 		std::cout << "getaddrinfo() error.\n";
// 		return 1;
// 	}

//     // connected for first result
// 	for(p = servinfo; p != NULL; p = p->ai_next) {
// 		if ((sockfd = socket(p->ai_family, p->ai_socktype,
// 							 p->ai_protocol)) == - 1) {
// 			perror("client: socket not connected");
// 			continue;
// 		}
//         // connect host
// 		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == - 1) {
// 			close(sockfd);
// 			perror("client: not connected");
// 			continue;
// 		}
// 		break;
// 	}

// 	if (p == NULL) {
// 		std::cout <<"client: failed to connecting" << std::endl;
// 		return 2;
// 	}
//     // from ip to str
// 	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
// 			  s, sizeof s);
// 	std::cout << "bot: connecting to " << s << std::endl;

// 	freeaddrinfo(servinfo); // clear

// 	std::string stroka;
	
// 	std::string str = "USER bot bot ";
// 	str.append(argv[3]);
// 	str.append("\r\nJOIN #bot\r\n");
	
// 	send(sockfd, str.c_str(), str.size() + 1, 0);
// 	while(1)
// 	{
// 		if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
// 		{
// 			perror("recv");
// 			exit(1);
// 		}
// 		buf[numbytes] = '\0';
// 		std::cout << "client: received " << buf << std::endl;
// 		stroka = buf;

// 		std::string response;

// 		response = botResponse(stroka);
// 		send(sockfd, response.c_str(), strlen(response.c_str()), 0);

// 		// if (stroka.find("[bot]: hi") != std::string::npos)
// 		// {
// 		// 	send(sockfd, "MSG #bot :Hello im a so small and cute bot! I can't do anything yet, but maybe someday I will learn interesting commands! BUT! Even now I can tell you how beautiful you are!\r\n", 179, 0);
// 		// }
// 		// else if (stroka.find("[bot]: time") != std::string::npos)
// 		// {
// 		// 	time_t rawtime;
//   		// 	struct tm * timeinfo;
//   		// 	char buffer[80];

//   		// 	time (&rawtime);
//   		// 	timeinfo = localtime(&rawtime);

//   		// 	strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
//   		// 	std::string times = "MSG #bot : current time: ";
// 		// 	times.append(buffer);
// 		// 	times.append("\r\n");
// 		// 	std::cout << times << std::endl;
// 		// 	send(sockfd, times.c_str(), times.size() + 1, 0); 
// 		// }
// 		stroka.clear();
// 	}

// 	close(sockfd);

// 	return 0;
// }
