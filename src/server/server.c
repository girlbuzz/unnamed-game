#include "server.h"
#include <stdlib.h>

int init_server(struct server *srv) {
	srv->clients_size = 0;
	srv->clients_capacity = 8;
	srv->clients = calloc(sizeof(struct connection), srv->clients_capacity);
}

void free_server(struct server *srv) {
	free(srv->clients);
}
