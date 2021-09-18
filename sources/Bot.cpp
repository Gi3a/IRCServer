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
	const int count = 5;
	std::string arrGreet[count] = {
		"hi",
		"hello",
		"privet",
		"zdarova",
		"hey"
	};

	std::string arrCita[count] = {
		"i dont know",
		"kogda mne bilo rovno 5",
		"world peace",
		"people, help the peolpe",
		"buy apricots"
	};

	std::string arrName[count] = 
	{
		"broooo",
		"dude",
		"stranger",
		"human",
		"honey"
	};
	int index = count - 1;
	if (str.find("bot hi") != std::string::npos)
	{
		response = arrGreet[rand() % index] + ", " + arrName[rand() % index];
	}
	else if (str.find("bot say") != std::string::npos)
	{
		response = arrCita[rand() % index] + ", " + arrName[rand() % index];
	}
	response += "\r\n";
}