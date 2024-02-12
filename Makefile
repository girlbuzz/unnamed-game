
SERVER=src/server/server.o src/server/ndb.o
CLIENT=src/client/client.o

# -I. is for the config.h file
CFLAGS += -Iinclude -I.

.PHONY: all clean
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: server client

server: $(SERVER)
	$(CC) $(LDFLAGS) -o $@ $(SERVER)

client: $(CLIENT)
	$(CC) $(LDFLAGS) -o $@ $(CLIENT)

clean:
	$(RM) $(SERVER) $(CLIENT) server client
