#ifndef COMMAND_H_
#define COMMAND_H_


class Command
{
protected:
	short opcode;

public:
	Command(short opcode);
	virtual short getOpcode();
	virtual ~Command();
};

#endif
