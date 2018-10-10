#include "CommandBcast.h"

using namespace std;

CommandBcast::CommandBcast(const string &filename, char type) : Command((short)9), filename(filename), type(type){}

string CommandBcast::getFilename(){
	return filename;
}

char CommandBcast::getType(){
	return type;
}

CommandBcast::~CommandBcast(){

}
