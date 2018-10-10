#ifndef COMMANDBCAST_H_
#define COMMANDBCAST_H_

#include "Command.h"
#include <string>

class CommandBcast : public Command
{
private:
	std::string filename;
	char type = 0;
public:
	CommandBcast(const std::string &filename, char type);
	std::string getFilename();
	char getType();
	virtual ~CommandBcast();

};

#endif
