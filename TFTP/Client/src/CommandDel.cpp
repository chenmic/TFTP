#include "../include/CommandDel.h"

using namespace std;

CommandDel::CommandDel(const string &filename) : Command((short)8), filename(filename){}

string CommandDel::getStr(){
	return filename;
}

CommandDel::~CommandDel(){}
