#ifndef COMMANDWRITE_H_
#define COMMANDWRITE_H_

#include "Command.h"
#include <string>

class CommandWrite : public Command
{
private:
	std::string filename;
public:
	CommandWrite(const std::string &filename);
	std::string getStr();
	virtual ~CommandWrite();
};

#endif
