#ifndef PACKETENCODERDECODER_H_
#define PACKETENCODERDECODER_H_

#include <string>
#include <vector>
#include "Command.h"
#include "CommandAck.h"
#include "CommandBcast.h"
#include "CommandData.h"
#include "CommandDel.h"
#include "CommandDir.h"
#include "CommandDisc.h"
#include "CommandError.h"
#include "CommandLog.h"
#include "CommandRead.h"
#include "CommandWrite.h"

class PacketEncoderDecoder{
private:
	char* opcodeArr;
	short opcode;
	int opcodeIndex;
	std::vector<char>* content;
	int conIndex;
	short dataSize;
	void pushByte(char nextByte);
	void clear();

public:
	PacketEncoderDecoder();
	PacketEncoderDecoder(const PacketEncoderDecoder& other);
	PacketEncoderDecoder &operator=(const PacketEncoderDecoder& other);
	Command* decodeNextByte(char nextByte);
	std::string encode(Command *message);
	short bytesToShort(char* bytesArr);
	void shortToBytes(short num, char* bytesArr);
	virtual ~PacketEncoderDecoder();

};

#endif
