
LIB=src/lib/timing.o
SERVER=src/server/main.o src/server/server.o src/server/game.o
CLIENT=src/client/main.o

CFLAGS += -Isrc/include

.PHONY: all clean
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: server client

server: $(SERVER) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $(SERVER) $(LIB)

client: $(CLIENT)
	$(CC) $(LDFLAGS) -o $@ $(CLIENT) $(LIB)

clean:
	$(RM) $(SERVER) $(CLIENT) $(LIB) server client
