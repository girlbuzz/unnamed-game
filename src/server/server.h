#ifndef __SERVER_H
#define __SERVER_H

#include <stddef.h>

#include <poll.h>

#include <player.h>

struct client {
	struct player player;
};

int init_client(struct client *);

struct server {
	/* client handling */
	struct client *clients;

	/* poll handling */
	struct pollfd *pollfds;

	/* this information is shared by clients and pollfds */
	size_t size;
	size_t capacity;

	/* timing */
	unsigned int tick;
};

int init_server(struct server *, int socket);
void free_server(struct server *);

int handle_inbound_connections(struct server *);
int handle_client_inputs(struct server *);

void update_game_state(struct server *);
void send_game_state(struct server *);

#endif /* __SERVER_H */
