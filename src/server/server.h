#ifndef __SERVER_H
#define __SERVER_H

#include <stddef.h>

struct connection {
	int fd;
};

struct server {
	struct connection *clients;
	size_t clients_size;
	size_t clients_capacity;
};

int init_server(struct server *);
void free_server(struct server *);

#endif /* __SERVER_H */
