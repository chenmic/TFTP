#include "ServerConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ServerConnectionHandler::ServerConnectionHandler(tcp::socket *socket): encdec_(),socket_(socket), keyboardLastCommand_(), file_(), dataStr_(), loggedIn_(false), shouldTerminate_(false), dataBlock_(1), ackBlock_(0), fileSize_(0){
	encdec_ = new PacketEncoderDecoder();
}

ServerConnectionHandler::ServerConnectionHandler(const ServerConnectionHandler& other): encdec_(),socket_(other.socket_), keyboardLastCommand_(other.keyboardLastCommand_), file_(), dataStr_(other.dataStr_), loggedIn_(other.loggedIn_), shouldTerminate_(other.shouldTerminate_), dataBlock_(other.dataBlock_), ackBlock_(other.ackBlock_), fileSize_(other.fileSize_){
	encdec_ = new PacketEncoderDecoder(*other.encdec_);
}

ServerConnectionHandler& ServerConnectionHandler::operator=(const ServerConnectionHandler& other){
	if (this!=&other){
		socket_ = other.socket_;
		keyboardLastCommand_ = other.keyboardLastCommand_;
		dataStr_ = other.dataStr_;
		loggedIn_ = other.loggedIn_;
		shouldTerminate_ = other.shouldTerminate_;
		dataBlock_ = other.dataBlock_;
		ackBlock_ = other.ackBlock_;
		fileSize_ = other.fileSize_;
		PacketEncoderDecoder* temp = new PacketEncoderDecoder(*other.encdec_);
		delete encdec_;
		encdec_ = temp;
	}
	return *this;
}

void ServerConnectionHandler::updateLastCommand(Command* command){
	this->keyboardLastCommand_ = command;
}

bool ServerConnectionHandler::getLoggedIn(){
	return loggedIn_;
}

bool ServerConnectionHandler::getShouldTerminate(){
	return shouldTerminate_;
}

void ServerConnectionHandler::run(){
	char byte;
	while(!shouldTerminate_){
		if(getBytes(&byte,1)){
			Command* fromSrv = encdec_->decodeNextByte(byte);
			if (fromSrv!=nullptr){
				process(fromSrv);
			}
		}
		else{
			sendCommand(new CommandError((short)0,"Not defined, see error message.\0"));
		}
	}
}

void ServerConnectionHandler::process(Command* fromSrv){
	switch(fromSrv->getOpcode()){
		case(3): //Data
			if (keyboardLastCommand_->getOpcode()==6){ // If it was a Dirq
				if ((dynamic_cast<CommandData*>(fromSrv))->getBlockNumber()==dataBlock_){
					if ((dynamic_cast<CommandData*>(fromSrv))->getPacketSize()<512){ // Last data packet.
						dataStr_.append((dynamic_cast<CommandData*>(fromSrv))->getData(),(dynamic_cast<CommandData*>(fromSrv))->getPacketSize());
						sendCommand(new CommandAck(dataBlock_));
						size_t loc = 0;
						std::string token;
						while ((loc = dataStr_.find('\0')) != std::string::npos) {
						    token = dataStr_.substr(0, loc);
						    std::cout << token << std::endl;
						    dataStr_.erase(0, loc + 1);
						}
						if (dataStr_.length()>0)
							std::cout << dataStr_ << std::endl;
						dataBlock_ = 1;
						delete keyboardLastCommand_;
						keyboardLastCommand_ = nullptr;
						dataStr_.clear();
					}
					else{ // All packets excluding the last.
						dataStr_.append((dynamic_cast<CommandData*>(fromSrv))->getData(),512);
						sendCommand(new CommandAck(dataBlock_++));
					}
				}
				else{
					if (file_.is_open())
						file_.close();
					delete keyboardLastCommand_;
					keyboardLastCommand_ = nullptr;
					dataStr_.clear();
					fileSize_=0;
					ackBlock_=0;
					dataBlock_=1;
					sendCommand(new CommandError((short)0,"Not defined, see error message.\0"));
				}
			}
			else if (keyboardLastCommand_->getOpcode()==1){ // If it is a RRQ
				if ((dynamic_cast<CommandData*>(fromSrv))->getBlockNumber()==dataBlock_){
					if (dataBlock_==1){
						file_.open((dynamic_cast<CommandRead*>(keyboardLastCommand_))->getStr().c_str(),std::fstream::out);
					}
					if ((dynamic_cast<CommandData*>(fromSrv))->getPacketSize()<512){ // Last data packet.
						if (file_.is_open()){
							file_.write((dynamic_cast<CommandData*>(fromSrv))->getData(),(dynamic_cast<CommandData*>(fromSrv))->getPacketSize());
							file_.close();
							sendCommand(new CommandAck(dataBlock_));
							std::cout << "RRQ " << (dynamic_cast<CommandRead*>(keyboardLastCommand_))->getStr() << " complete" << std::endl;
							dataBlock_ = 1;
						}
						delete keyboardLastCommand_;
						keyboardLastCommand_ = nullptr;
					}
					else{ // All packets excluding the last.
						if (file_.is_open()){
							file_.write((dynamic_cast<CommandData*>(fromSrv))->getData(),512);
							sendCommand(new CommandAck(dataBlock_));
							dataBlock_++;
						}
					}
				}
				else{
					if (file_.is_open())
						file_.close();
					delete keyboardLastCommand_;
					keyboardLastCommand_ = nullptr;
					dataStr_.clear();
					fileSize_=0;
					ackBlock_=0;
					dataBlock_=1;
					sendCommand(new CommandError((short)0,"Not defined, see error message.\0"));
				}
			}
			break;
		case(4): // Ack
			std::cout << "ACK " << (dynamic_cast<CommandAck*>(fromSrv))->getBlockNumber() << endl;
			if ((dynamic_cast<CommandAck*>(fromSrv))->getBlockNumber()==ackBlock_){
				if (keyboardLastCommand_->getOpcode()==10){
					shouldTerminate_=true;
					delete keyboardLastCommand_;
					keyboardLastCommand_ = nullptr;
				}else if (keyboardLastCommand_->getOpcode()==7){
					loggedIn_=true;
					delete keyboardLastCommand_;
					keyboardLastCommand_ = nullptr;
				}
				else if (keyboardLastCommand_->getOpcode()==2){
					if(ackBlock_==0){
						file_.open((dynamic_cast<CommandWrite*>(keyboardLastCommand_))->getStr().c_str(),std::fstream::in);
						if (file_.is_open()){
							file_.seekg(0,std::fstream::end);
							fileSize_ = file_.tellg();
							file_.seekg(0,std::fstream::beg);
						}
					}
					if (fileSize_>=512){
						if (file_.is_open()){
							char* readTemp = new char[512];
							file_.read(readTemp,512);
							fileSize_-=512;
							sendCommand(new CommandData((short)512,++ackBlock_,readTemp));
						}
					}
					else if (fileSize_>=0){ // Last Packet
							if (file_.is_open()){
								char* readTemp = new char[fileSize_];
								file_.read(readTemp,fileSize_);
								sendCommand(new CommandData((short)fileSize_,++ackBlock_,readTemp));
								fileSize_=-1;
							}
						}
						else{
							std::cout << "WRQ " << (dynamic_cast<CommandWrite*>(keyboardLastCommand_))->getStr() << " complete" << std::endl;
							file_.close();
							fileSize_ = 0;
							ackBlock_ = 0;
							delete keyboardLastCommand_;
							keyboardLastCommand_ = nullptr;
						}
				}
			}
			else{
				if (file_.is_open())
					file_.close();
				delete keyboardLastCommand_;
				keyboardLastCommand_ = nullptr;
				dataStr_.clear();
				fileSize_=0;
				ackBlock_=0;
				dataBlock_=1;
				sendCommand(new CommandError((short)0,"Not defined, see error message.\0"));
			}
			break;
		case(5): // Error
				std::cout << "Error " << (dynamic_cast<CommandError*>(fromSrv))->getErrorCode() << std::endl;
				if (file_.is_open())
					file_.close();
				delete keyboardLastCommand_;
				keyboardLastCommand_ = nullptr;
				dataStr_.clear();
				fileSize_= 0;
				ackBlock_= 0;
				dataBlock_=1;
			break;
		case(9):{ // Bcast
			std::string bcastType;
			if ((dynamic_cast<CommandBcast*>(fromSrv))->getType()=='\1')
				bcastType = "add";
			else
				bcastType = "del";
			std::cout << "BCAST " << bcastType << " " << (dynamic_cast<CommandBcast*>(fromSrv))->getFilename() << std::endl;
			delete keyboardLastCommand_;
			keyboardLastCommand_ = nullptr;
			break;
		}
		default:
			delete keyboardLastCommand_;
			keyboardLastCommand_ = nullptr;
			break;
	}
	delete fromSrv;
}

ServerConnectionHandler::~ServerConnectionHandler() {
	delete encdec_;
}

bool ServerConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_->read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        return false;
    }
    return true;
}

bool ServerConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
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

bool ServerConnectionHandler::sendCommand(Command* command) {
	if (command==nullptr){
		return false;
	}
	std::string s = encdec_->encode(command);
	delete command;
	return sendBytes(s.c_str(),s.length());

}
