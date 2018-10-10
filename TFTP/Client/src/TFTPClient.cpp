#include <stdlib.h>
#include <boost/thread.hpp>
#include "../include/KeyBConnectionHandler.h"
#include "../include/ServerConnectionHandler.h"

int main (int argc, char *argv[]) {
    if (argc < 3)
        return -1;
    std::string host = argv[1];
    short port = atoi(argv[2]);
	boost::asio::io_service ioservice;
	boost::asio::ip::tcp::socket socket(ioservice);
	try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
		boost::system::error_code error;
		socket.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
	}
	catch (std::exception& e) {
		std::cerr << "Cannot connect to " << host << ":" << port << " (Error: " << e.what() << ')' << std::endl;
		return 1;
	}

    ServerConnectionHandler *server = new ServerConnectionHandler(&socket);
    KeyBConnectionHandler *keyboard = new KeyBConnectionHandler(&socket,server);
    boost::thread *th1 = new boost::thread(boost::bind(&ServerConnectionHandler::run,server));
    boost::thread *th2 = new boost::thread(boost::bind(&KeyBConnectionHandler::run,keyboard));

    th2->join();
    th1->join();
    delete server;
    delete keyboard;
    delete th1;
    delete th2;
    socket.close();
    return 0;
}

