#include "../include/CommandLog.h"

using namespace std;

CommandLog::CommandLog(const string &username) : Command((short)7), username(username){}

string CommandLog::getStr(){
	return username;
}

CommandLog::~CommandLog(){}
