#include <timing.h>
#include <config/server.h>

unsigned int ms(void) {
	struct timespec spec;

	clock_gettime(CLOCK_MONOTIC, &spec);

	return spec.tv_sec * 1000 + spec.tv_nsec / 1e6;
}

unsigned int tick(void) {
	return 1000.0 / TICKS_PER_SEC;
}
