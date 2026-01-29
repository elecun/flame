# Makefile for ZPipe Examples

CC = g++
CXXFLAGS = -O3 -Wall -std=c++17 -I../include -I../include/dep -I../include/dep/libzmq
LDFLAGS = -lpthread -lzmq

# Source Files
ZPIPE_SRC = ../include/flame/pipe/zpipe.cc
ZPIPE_OBJ = zpipe.o

all: zpipe_pub zpipe_sub

zpipe.o: $(ZPIPE_SRC)
	$(CC) $(CXXFLAGS) -c $(ZPIPE_SRC) -o $(ZPIPE_OBJ)

zpipe_pub: zpipe_pub.cc $(ZPIPE_OBJ)
	$(CC) $(CXXFLAGS) zpipe_pub.cc $(ZPIPE_OBJ) -o zpipe_pub $(LDFLAGS)

zpipe_sub: zpipe_sub.cc $(ZPIPE_OBJ)
	$(CC) $(CXXFLAGS) zpipe_sub.cc $(ZPIPE_OBJ) -o zpipe_sub $(LDFLAGS)

clean:
	rm -f zpipe_pub zpipe_sub *.o
