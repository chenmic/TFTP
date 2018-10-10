#ifndef COMMANDERROR_H_
#define COMMANDERROR_H_

#include "Command.h"
#include <string>

class CommandError : public Command
{
private:
	short errorCode;
	std::string errorMsg;

public:
	CommandError(short errorCode,std::string errorMsg);
	std::string getErrorMsg();
	short getErrorCode();
	virtual ~CommandError();

};


#endif
