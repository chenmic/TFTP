#ifndef KEYB_CONNECTION_HANDLER__
#define KEYB_CONNECTION_HANDLER__
                                           
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "PacketEncoderDecoder.h"
#include "ServerConnectionHandler.h"

using boost::asio::ip::tcp;

class KeyBConnectionHandler {
private:
	PacketEncoderDecoder *encdec_;
	ServerConnectionHandler *server_;
	tcp::socket *socket_;
	bool exist(Command* command);
 
public:
	KeyBConnectionHandler(tcp::socket *socket, ServerConnectionHandler *server);
	KeyBConnectionHandler(const KeyBConnectionHandler& other);
	KeyBConnectionHandler &operator=(const KeyBConnectionHandler& other);
	Command* keyboardDecode(std::string& line);
	void run();
    virtual ~KeyBConnectionHandler();

	// Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);
	
	// Send an ascii line from the server
    // Returns false in case connection closed before all the data is sent.
    bool sendLine(std::string& line);
 
}; //class ConnectionHandler
 
#endif
