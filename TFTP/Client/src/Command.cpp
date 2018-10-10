#include "Command.h"

Command::Command(short opcode) : opcode(opcode){}

short Command::getOpcode(){
	return opcode;
}

Command::~Command(){}
