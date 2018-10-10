#ifndef COMMANDDELETE_H_
#define COMMANDDELETE_H_

#include "Command.h"
#include <string>

class CommandDel : public Command
{

private:
	std::string filename;
public:
	CommandDel(const std::string &filename);
	std::string getStr();
	virtual ~CommandDel();
};

#endif

