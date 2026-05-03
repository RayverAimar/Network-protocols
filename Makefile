CXX      := g++
CXXFLAGS := -std=c++17 -Wall
LDFLAGS  := -lpthread

.PHONY: all clean

all: server client

server: src/server.cpp include/utils.h include/client.h include/TicTacToe.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

client: src/client.cpp include/utils.h include/TicTacToe.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f server client
