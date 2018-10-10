#include "CommandWrite.h"

using namespace std;

CommandWrite::CommandWrite(const string &filename) : Command((short)2), filename(filename){}

string CommandWrite::getStr(){
	return filename;
}

CommandWrite::~CommandWrite(){}
