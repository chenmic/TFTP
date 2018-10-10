#include "KeyBConnectionHandler.h"
 
using boost::asio::ip::tcp;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 
KeyBConnectionHandler::KeyBConnectionHandler(tcp::socket *socket, ServerConnectionHandler *server): encdec_(),server_(server), socket_(socket){
	encdec_ = new PacketEncoderDecoder();
}

KeyBConnectionHandler::KeyBConnectionHandler(const KeyBConnectionHandler& other): encdec_(),server_(other.server_), socket_(other.socket_){
	encdec_ = new PacketEncoderDecoder(*other.encdec_);
}

KeyBConnectionHandler& KeyBConnectionHandler::operator=(const KeyBConnectionHandler& other){
	if (this!=&other){
		server_=other.server_;
		socket_=other.socket_;
		PacketEncoderDecoder* temp = new PacketEncoderDecoder(*other.encdec_);
		delete encdec_;
		encdec_ = temp;
	}
	return *this;
}

bool KeyBConnectionHandler::exist(Command* command){
	if (command->getOpcode()==1){
		std::ifstream file((dynamic_cast<CommandRead*>(command))->getStr());
		if (file){
			std::cout << "File already exist." << std::endl;
			file.close();
			return true;
		}
	}
	else if(command->getOpcode()==2){
		std::ifstream file((dynamic_cast<CommandWrite*>(command))->getStr());
			if (!file){
				std::cout << "File doesn't exist." << std::endl;
				return true;
			}
		}
	return false;
}

Command* KeyBConnectionHandler::keyboardDecode(std::string& line){
	Command* ans = nullptr;
	if (line.find(' ') == std::string::npos){ // no spaces in string - meaning Dir / Disc / possibly bad input
		if(line.compare("DIRQ")==0)
			ans = new CommandDir();
		else{
			if(line.compare("DISC")==0)
				ans = new CommandDisc();
		}
	}
	else{
		std::string type = line.substr(0,line.find(' '));
		std::string name = line.substr(line.find(' ')+1,line.length());
		if (name.find(' ') == std::string::npos){
			if(type.compare("DELRQ")==0)
				ans = new CommandDel(name);
			else{
				if(type.compare("LOGRQ")==0)
					ans = new CommandLog(name);
				else{
					if(type.compare("RRQ")==0)
						ans = new CommandRead(name);
					else{
						if(type.compare("WRQ")==0)
							ans = new CommandWrite(name);
					}
				}
			}
		}
	}
	return ans;
}
void KeyBConnectionHandler::run() {
	while (!server_->getShouldTerminate()) {
		const short bufsize = 1024;
		char buf[bufsize];
		std::cin.getline(buf, bufsize);
		std::string line(buf);
		if(!sendLine(line)){
			std::cout << "Entered an invalid input. Please try again." << std::endl;
		}
		if (server_->getLoggedIn() && line.compare("DISC")==0)
			break;
    }
}
    
KeyBConnectionHandler::~KeyBConnectionHandler() {
	delete encdec_;
}

bool KeyBConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_->write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        return false;
    }
    return true;
}

bool KeyBConnectionHandler::sendLine(std::string& line) {
	Command* toSend = keyboardDecode(line);
	if (toSend==nullptr || exist(toSend)){
		delete toSend;
		return false;
	}
	string s = encdec_->encode(toSend);
	server_->updateLastCommand(toSend);
	if (!sendBytes(s.c_str(),s.length())){
		return false;
	}
	if (toSend->getOpcode()==6 || toSend->getOpcode()==10)
		return true;
	char delimiter = '\0';
	return sendBytes(&delimiter,1);
}
