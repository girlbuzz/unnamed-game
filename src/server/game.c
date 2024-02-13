#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <config/server.h>
#include <timing.h>

#include "game.h"
#include "server.h"

int game(int socket) {
	unsigned int t = 0;
	struct server srv;

	if (init_server(&srv, socket)) {
		fprintf(stderr, "error: failed to initialize server: %s\n", strerror(errno));
		return 1;
	}

	for (;;) {
		unsigned int now = ms();
		int ready;

		if (t >= srv.tick) {
timeout:
			update_game_state(&srv);
			send_game_state(&srv);
			t = 0;
		}

		ready = poll(srv.pollfds, srv.size + 1, srv.tick - t);
		t += ms() - now;

		if (ready < 0) {
			fprintf(stderr, "error: poll() :%s\n", strerror(errno));
			return 1;
		}

		if (ready == 0)
			goto timeout;

		if (handle_inbound_connections(&srv))
			fprintf(stderr, "warn: failed to handle connection: %s\n", strerror(errno));

		if (handle_client_inputs(&srv))
			fprintf(stderr, "warn: failed to handle client inputs: %s\n", strerror(errno));
	}

	return 0;
}
