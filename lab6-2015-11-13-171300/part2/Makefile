
#
# Makefile for lab 6, part 1
#

CC  = gcc
CXX = g++

INCLUDES = -I../../lab3/part1
CFLAGS   = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g -L../../lab3/part1
LDLIBS = -lmylist

.PHONY: default
default: http-client


http-client: http-client.o  

# header dependency
http-client.o:

.PHONY: clean
clean:
	rm -f *.o *~ a.out core http-client 

.PHONY: all
all: clean default

