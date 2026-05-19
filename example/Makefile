# Makefile for ZPipe Examples

CC = g++
CXXFLAGS = -O3 -Wall -std=c++20 -I../include -I../include/dep -I../include/dep/libzmq
LDFLAGS = -lpthread -lzmq

# Source Files
ZPIPE_SRC = ../include/flame/common/zpipe.cc
ZPIPE_OBJ = zpipe.o

TARGETS = zpipe_pub zpipe_sub zpipe_push zpipe_pull

all: $(TARGETS)

zpipe.o: $(ZPIPE_SRC)
	$(CC) $(CXXFLAGS) -c $(ZPIPE_SRC) -o $(ZPIPE_OBJ)

zpipe_pub: zpipe_pub.cc $(ZPIPE_OBJ)
	$(CC) $(CXXFLAGS) zpipe_pub.cc $(ZPIPE_OBJ) -o zpipe_pub $(LDFLAGS)

zpipe_sub: zpipe_sub.cc $(ZPIPE_OBJ)
	$(CC) $(CXXFLAGS) zpipe_sub.cc $(ZPIPE_OBJ) -o zpipe_sub $(LDFLAGS)

zpipe_push: zpipe_push.cc $(ZPIPE_OBJ)
	$(CC) $(CXXFLAGS) zpipe_push.cc $(ZPIPE_OBJ) -o zpipe_push $(LDFLAGS)

zpipe_pull: zpipe_pull.cc $(ZPIPE_OBJ)
	$(CC) $(CXXFLAGS) zpipe_pull.cc $(ZPIPE_OBJ) -o zpipe_pull $(LDFLAGS)

clean:
	rm -f $(TARGETS) *.o
