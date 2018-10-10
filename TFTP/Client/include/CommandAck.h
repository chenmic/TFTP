#ifndef COMMANDACK_H_
#define COMMANDACK_H_

#include "Command.h"

class CommandAck : public Command
{
private:
	short blockNumber = 0;

public:
	CommandAck(short blockNumber);
	virtual short getBlockNumber();
	virtual ~CommandAck();



};

#endif


