#ifndef COMMANDDATA_H_
#define COMMANDDATA_H_

#include "Command.h"
#include <vector>

class CommandData : public Command
{
private:
	short packetSize;
	short blockNumber;
	char* data;
	CommandData(const CommandData& other);
	CommandData& operator=(const CommandData& other);
public:
	CommandData(short packetSize, short blockNumber, char* data);
	short getPacketSize();
	short getBlockNumber();
	char* getData();
	virtual ~CommandData();

};

#endif

