#ifndef COMMANDREAD_H_
#define COMMANDREAD_H_

#include "Command.h"
#include <string>


class CommandRead : public Command
{
private:
	std::string filename;

public:
	CommandRead(const std::string &filename);
	std::string getStr();
	virtual ~CommandRead();
};


#endif
