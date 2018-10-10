#ifndef COMMANDLOGIN_H_
#define COMMANDLOGIN_H_

#include "Command.h"
#include <string>




class CommandLog : public Command
{
private:
	std::string username;

public:
	CommandLog(const std::string &username);
	std::string getStr();
	virtual ~CommandLog();
};


#endif
