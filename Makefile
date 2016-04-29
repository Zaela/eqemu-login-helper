
CXX?= g++
CFLAGS= -O2 -Wall -Wno-strict-aliasing
BINDIR= ./bin/
SRCDIR= ./src/
OBJDIR= ./build/

# Uncomment to send raw packet output to stdout and enable debugging
#CFLAGS+= -D_DEBUG -O0 -g

# Define a custom port to point the client to here (as an integer)
#CFLAGS+= -DMIDDLEMAN_PORT=5998

# Define custom login server hostname and port here (both as strings)
#CFLAGS+= -DREMOTE_HOST=\"login.eqemulator.net\"
#CFLAGS+= -DREMOTE_PORT=\"5998\"

_OBJS= main.o connection.o packet_queue.o
_DEPS= connection.hpp packet_queue.hpp

OBJS= $(patsubst %,$(OBJDIR)%,$(_OBJS))
DEPS= $(patsubst %,$(SRCDIR)%,$(_DEPS))

BIN= $(BINDIR)eqemu-login-helper

default all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(CFLAGS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)*.o
	rm -f $(BIN)

