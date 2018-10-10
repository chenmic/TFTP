#include "CommandData.h"

CommandData::CommandData(short packetSize, short blockNumber, char* data) : Command((short)3), packetSize(packetSize), blockNumber(blockNumber), data(data){}

CommandData::CommandData(const CommandData& other):Command((short)3), packetSize(other.packetSize), blockNumber(other.blockNumber), data(other.data){}

CommandData& CommandData::operator=(const CommandData& other){
	if (this!=&other){
		packetSize = other.packetSize;
		blockNumber = other.blockNumber;
		char* temp = other.data;
		delete[] data;
		data = temp;
	}
	return *this;
}

short CommandData::getPacketSize(){
	return packetSize;
}

short CommandData::getBlockNumber(){
	return blockNumber;
}

char* CommandData::getData(){
	return data;
}

CommandData::~CommandData(){
	delete[] data;
}
