BIN = bin/TFTPClient.o bin/PacketEncoderDecoder.o bin/KeyBConnectionHandler.o bin/ServerConnectionHandler.o bin/Command.o bin/CommandAck.o bin/CommandBcast.o bin/CommandData.o bin/CommandDel.o bin/CommandDir.o bin/CommandDisc.o bin/CommandError.o bin/CommandLog.o bin/CommandRead.o bin/CommandWrite.o
RM = rm -f bin/*
CFLAGS  = -Wall -Weffc++ -g -std=c++11 -Iinclude -c
LDFLAGS  = -lboost_system -lboost_locale -lboost_thread

# All Targets
all: clean $(BIN)
	g++ -o bin/TFTPClient $(BIN) $(LDFLAGS)

# Dependencies
bin/TFTPClient.o: src/TFTPClient.cpp
	g++ $(CFLAGS) -o bin/TFTPClient.o src/TFTPClient.cpp
 
bin/PacketEncoderDecoder.o: src/PacketEncoderDecoder.cpp
	g++ $(CFLAGS) -o bin/PacketEncoderDecoder.o src/PacketEncoderDecoder.cpp
	
bin/KeyBConnectionHandler.o: src/KeyBConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/KeyBConnectionHandler.o src/KeyBConnectionHandler.cpp
	
bin/ServerConnectionHandler.o: src/ServerConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ServerConnectionHandler.o src/ServerConnectionHandler.cpp
	
bin/Command.o: src/Command.cpp
	g++ $(CFLAGS) -o bin/Command.o src/Command.cpp
	
bin/CommandAck.o: src/CommandAck.cpp
	g++ $(CFLAGS) -o bin/CommandAck.o src/CommandAck.cpp
	
bin/CommandBcast.o: src/CommandBcast.cpp
	g++ $(CFLAGS) -o bin/CommandBcast.o src/CommandBcast.cpp
	
bin/CommandData.o: src/CommandData.cpp
	g++ $(CFLAGS) -o bin/CommandData.o src/CommandData.cpp
	
bin/CommandDel.o: src/CommandDel.cpp
	g++ $(CFLAGS) -o bin/CommandDel.o src/CommandDel.cpp

bin/CommandDir.o: src/CommandDir.cpp
	g++ $(CFLAGS) -o bin/CommandDir.o src/CommandDir.cpp

bin/CommandDisc.o: src/CommandDisc.cpp
	g++ $(CFLAGS) -o bin/CommandDisc.o src/CommandDisc.cpp
	
bin/CommandError.o: src/CommandError.cpp
	g++ $(CFLAGS) -o bin/CommandError.o src/CommandError.cpp
	
bin/CommandLog.o: src/CommandLog.cpp
	g++ $(CFLAGS) -o bin/CommandLog.o src/CommandLog.cpp
	
bin/CommandRead.o: src/CommandRead.cpp
	g++ $(CFLAGS) -o bin/CommandRead.o src/CommandRead.cpp
	
bin/CommandWrite.o: src/CommandWrite.cpp
	g++ $(CFLAGS) -o bin/CommandWrite.o src/CommandWrite.cpp

#Cleans the binaries folder
.PHONY: clean
clean:
	$(RM)

