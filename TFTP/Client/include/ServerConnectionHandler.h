#ifndef SERVERCONNECTIONHANDLER_H_
#define SERVERCONNECTIONHANDLER_H_

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include "PacketEncoderDecoder.h"

using boost::asio::ip::tcp;

class ServerConnectionHandler {
private:
	PacketEncoderDecoder *encdec_;
	tcp::socket *socket_;
	Command* keyboardLastCommand_;
	std::fstream file_;
	std::string dataStr_;
	bool loggedIn_;
	bool shouldTerminate_;
	short dataBlock_;
	short ackBlock_;
	int fileSize_;

public:
	ServerConnectionHandler(tcp::socket *socket);
	ServerConnectionHandler(const ServerConnectionHandler& other);
	ServerConnectionHandler &operator=(const ServerConnectionHandler& other);
	void updateLastCommand(Command* command);
	bool getLoggedIn();
	void run();
	void process(Command* fromSrv);
    virtual ~ServerConnectionHandler();
    bool getShouldTerminate();

    // Read a fixed number of bytes from the server - blocking.
    // Returns false in case the connection is closed before bytesToRead bytes can be read.
    bool getBytes(char bytes[], unsigned int bytesToRead);

	// Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);

	// Send an ascii line from the server
    // Returns false in case connection closed before all the data is sent.
    bool sendCommand(Command* command);

};

#endif
