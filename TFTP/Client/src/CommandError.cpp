#include "CommandError.h"

using namespace std;

CommandError::CommandError(short errorCode, std::string errorMsg) : Command((short)5), errorCode(errorCode), errorMsg(errorMsg){}

string CommandError::getErrorMsg(){
	return errorMsg;
}

short CommandError::getErrorCode(){
	return errorCode;
}

CommandError::~CommandError(){}
