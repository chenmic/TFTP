#include "CommandAck.h"

CommandAck::CommandAck(short blockNumber) : Command((short)4), blockNumber(blockNumber){}

short CommandAck::getBlockNumber(){
	return blockNumber;
}

CommandAck::~CommandAck(){}
