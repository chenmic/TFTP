#include "CommandRead.h"

using namespace std;

CommandRead::CommandRead(const string &filename) : Command((short)1), filename(filename){}

string CommandRead::getStr(){
	return filename;
}

CommandRead::~CommandRead(){}
