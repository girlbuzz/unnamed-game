#define _XOPEN_SOURCE 500
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>

#include <timing.h>

#include "server.h"

int init_client(struct client *client) {
	client->player.pos.x = client->player.pos.y = 0.0;
	return 0;
}

/* This technically doesn't need to exist. It could in the future, though.
 * And if it does eventually need to be called hopefully it will already be
 * in use everywhere it needs to be. */
void free_client(struct client *client) {
	(void) client;
}

int init_server(struct server *srv, int socket) {
	srv->size = 0;
	srv->capacity = 8;

	srv->clients = calloc(sizeof(struct client), srv->capacity);
	srv->pollfds = calloc(sizeof(struct pollfd), srv->capacity);

	srv->pollfds[0].fd = socket;
	srv->pollfds[0].events = POLLIN;

	srv->tick = tick();

	return 0;
}

void free_server(struct server *srv) {
	free(srv->clients);
	free(srv->pollfds);
}

int handle_inbound_connections(struct server *srv) {
	nfds_t i;

	if (srv->pollfds[0].revents & POLLIN) {
		int conn = accept(srv->pollfds[0].fd, NULL, NULL);

		if (conn < 0)
			return conn;

		for (i = 1; i < srv->size + 1; i++) {
			if (srv->pollfds[i].fd < 0) {
				srv->pollfds[i].fd = conn;
				srv->pollfds[i].events = POLLIN | POLLOUT;

				init_client(&srv->clients[i - 1]);

				return 0;
			}
		}

		if (srv->size == srv->capacity) {
			srv->capacity *= 2;
			srv->clients = realloc(srv->clients, sizeof(struct client) * srv->capacity);
			srv->pollfds = realloc(srv->pollfds, sizeof(struct pollfd) * (srv->capacity + 1));
		}

		srv->pollfds[srv->size + 1].fd = conn;
		srv->pollfds[srv->size + 1].events = POLLIN | POLLOUT;

		init_client(&srv->clients[srv->size]);

		srv->size++;
	}

	return 0;
}

int handle_client_inputs(struct server *srv) {
	(void) srv;
	return 0;
}

void update_game_state(struct server *srv) {
	(void) srv;
}

void send_game_state(struct server *srv) {
	size_t i;

	for (i = 0; i < srv->size; i++) {
		if (srv->pollfds[i + 1].revents & (POLLERR | POLLHUP)) {
			srv->pollfds[i + 1].fd = -1;
			free_client(&srv->clients[i]);
			continue;
		}

		if (srv->pollfds[i + 1].revents & POLLOUT)
			send(srv->pollfds[i + 1].fd, "hello!\n", 7, 0);
	}
}
