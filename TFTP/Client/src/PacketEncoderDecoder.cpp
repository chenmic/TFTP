#include "../include/PacketEncoderDecoder.h"
#include <string.h>
#include <stdio.h>

PacketEncoderDecoder::PacketEncoderDecoder():opcodeArr(), opcode(0), opcodeIndex(0), content(), conIndex(0), dataSize(0){
	opcodeArr = new char[2];
	content = new std::vector<char>();
}

PacketEncoderDecoder::PacketEncoderDecoder(const PacketEncoderDecoder& other):opcodeArr(), opcode(other.opcode), opcodeIndex(other.opcodeIndex), content(), conIndex(other.conIndex), dataSize(other.dataSize){
	opcodeArr = new char[2];
	opcodeArr[0] = other.opcodeArr[0];
	opcodeArr[1] = other.opcodeArr[1];
	content = new std::vector<char>(*other.content);
}

PacketEncoderDecoder& PacketEncoderDecoder::operator=(const PacketEncoderDecoder& other){
	if (this!=&other){
		opcode = other.opcode;
		opcodeIndex = other.opcodeIndex;
		conIndex = other.conIndex;
		dataSize = other.dataSize;
		char* temp = new char[2];
		std::vector<char>* tempVec = new std::vector<char>(*other.content);
		temp[0] = other.opcodeArr[0];
		temp[1] = other.opcodeArr[1];
		delete[] opcodeArr;
		delete content;
		opcodeArr = temp;
		content = tempVec;
	}
	return *this;
}

Command* PacketEncoderDecoder::decodeNextByte(char nextByte){
	if (opcodeIndex < 2){
		opcodeArr[opcodeIndex++] = nextByte;
		if (opcodeIndex == 2)
			opcode = bytesToShort(opcodeArr);
	}
	else{
		Command *ans;
		switch (opcode){
			case 3: //data
				if (conIndex < 2){
					pushByte(nextByte);
					if (conIndex == 2){
						char* dataSizeArr = new char[2];
						dataSizeArr[0] = content->at(0);
						dataSizeArr[1] = content->at(1);
						dataSize = bytesToShort(dataSizeArr);
						delete[] dataSizeArr;
					}
				}
				else{
					pushByte(nextByte);
					if (conIndex == dataSize + 4){
						char* dataBlockNumber = new char[2];
						dataBlockNumber[0] = content->at(2);
						dataBlockNumber[1] = content->at(3);
						char* data = new char[dataSize];
						std::copy(content->begin()+4,content->begin()+conIndex,data);
						ans = new CommandData(dataSize,bytesToShort(dataBlockNumber), data);
						delete[] dataBlockNumber;
						clear();
						return ans;
					}
				}
				break;
			case 4: //ack
				pushByte(nextByte);
				if (conIndex == 2){
					char* ackBlockNumber = new char[2];
					ackBlockNumber[0] = content->at(0);
					ackBlockNumber[1] = content->at(1);
					ans = new CommandAck(bytesToShort(ackBlockNumber));
					delete[] ackBlockNumber;
					clear();
					return ans;
				}
				break;
			case 5: //error
				if (nextByte == '\0' && conIndex>1){
					char* errorCode = new char[2];
					errorCode[0] = content->at(0);
					errorCode[1] = content->at(1);
					ans = new CommandError(bytesToShort(errorCode),std::string(content->begin()+2,content->begin()+conIndex));
					delete[] errorCode;
					clear();
					return ans;
				}
				pushByte(nextByte);
				break;
			case 9: //bcast
				if (nextByte == '\0' && conIndex>0){
					ans = new CommandBcast(std::string(content->begin()+1,content->begin()+conIndex),content->at(0));
					clear();
					return ans;
				}
				pushByte(nextByte);
				break;
		}
	}
	return nullptr;
}


std::string PacketEncoderDecoder::encode(Command *message){
	std::string temp;
	char* op = new char[2];
	shortToBytes(message->getOpcode(),op);
	switch (message->getOpcode()){
		case 1:{ //read
			temp = (dynamic_cast<CommandRead*>(message))->getStr();
			break;
		}
		case 2:{ //write
			temp = (dynamic_cast<CommandWrite*>(message))->getStr();
			break;
		}
		case 3:{ //data
			char* dataSize = new char[2];
			int size = (dynamic_cast<CommandData*>(message))->getPacketSize();
			shortToBytes(size,dataSize);
			char* dataBlockNum = new char[2];
			shortToBytes((dynamic_cast<CommandData*>(message))->getBlockNumber(),dataBlockNum);
			char* data = (dynamic_cast<CommandData*>(message))->getData();
			temp.insert(0,1,dataBlockNum[1]);
			temp.insert(0,1,dataBlockNum[0]);
			temp.insert(0,1,dataSize[1]);
			temp.insert(0,1,dataSize[0]);
			for (int i=0; i<size; i++){
				temp = temp + data[i];
			}
			delete[] dataSize;
			delete[] dataBlockNum;
			break;
		}
		case 4:{ //ack
			char* ackBlockNum = new char[2];
			shortToBytes((dynamic_cast<CommandAck*>(message))->getBlockNumber(),ackBlockNum);
			temp.insert(0,1,ackBlockNum[1]);
			temp.insert(0,1,ackBlockNum[0]);
			delete[] ackBlockNum;
			break;
		}
		case 5:{ //error
			char* errorCode = new char[2];
			shortToBytes((dynamic_cast<CommandError*>(message))->getErrorCode(),errorCode);
			temp = (dynamic_cast<CommandError*>(message))->getErrorMsg();
			temp.insert(0,1,errorCode[1]);
			temp.insert(0,1,errorCode[0]);
			delete[] errorCode;
			break;
		}
		case 7:{ //login
			temp = (dynamic_cast<CommandLog*>(message))->getStr();
			break;
		}
		case 8:{ //delete
			temp = (dynamic_cast<CommandDel*>(message))->getStr();
			break;
		}
	}
	temp.insert(0,1,op[1]);
	temp.insert(0,1,op[0]);
	delete[] op;
	return temp;
}

void PacketEncoderDecoder::pushByte(char nextByte){
	content->push_back(nextByte);
	conIndex++;
}


void PacketEncoderDecoder::clear(){
	content->clear();
	conIndex = 0;
	opcodeIndex = 0;
	opcode = 0;
	dataSize = 0;

}


short PacketEncoderDecoder::bytesToShort(char* bytesArr){
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

void PacketEncoderDecoder::shortToBytes(short num, char* bytesArr){
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

PacketEncoderDecoder::~PacketEncoderDecoder(){
	delete[] opcodeArr;
	delete content;
}
