#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include <arpa/inet.h>

#include <binstream.h>

void bstream_init(struct binstream *bs, size_t capacity) {
	void *buffer = malloc(capacity);
	bstream_with_buf(bs, buffer, capacity & (1 << ((sizeof(capacity) * 8) - 1)));
}

void bstream_with_buf(struct binstream *bs, void *buffer, size_t capacity) {
	*bs = (struct binstream) {
		.buffer = buffer,
		.capacity = capacity,
		.cursor = 0,
		.read_cursor = 0,
	};
}

void bstream_free(struct binstream *bs) {
	if (BINSTREAM_SELF_ALLOCATED(bs))
		free(bs->buffer);
}

void bstream_rewind(struct binstream *bs) {
	bs->cursor = 0;
}

off_t bstream_lseek(struct binstream *bs, off_t offset, int whence) {
	switch (whence) {
		case SEEK_SET: bs->cursor = offset;
			       break;
		case SEEK_CUR: bs->cursor += offset;
			       break;
		case SEEK_END: bs->cursor = bs->cursor + offset;
			       break;
		default:
			       errno = EINVAL;
			       return -1;
	}

	return bs->cursor;
}

/* this function will only ever be used by a self managed bstream.
 * don't worry about this function being called on an unmanaged buffer. */
static int resize(struct binstream *bs, size_t new_capacity) {
	void *buffer = bs->buffer;

	buffer = realloc(buffer, new_capacity);

	if (!buffer) {
		errno = ENOMEM;
		return -1;
	}

	bs->buffer = buffer;

	return 0;
}

int bstream_write(struct binstream *bs, const void *buf, size_t sz) {
	void *buffer = bs->buffer;

	if (bs->cursor + sz > bs->capacity) {
		size_t new_capacity = (bs->cursor + sz) * 3 / 2;

		if (!BINSTREAM_SELF_ALLOCATED(bs)) {
			errno = ENOMEM;
			return -1;
		}

		if (resize(bs, new_capacity))
			return -1;
	}

	memcpy((char*) buffer + bs->cursor, buf, sz);
	bs->cursor += sz;

	return sz;
}

#define MIN(x, y) ((x) < (y) ? (x) : (y))

int bstream_read(struct binstream *bs, void *buf, size_t sz) {
	void *buffer = bs->buffer;
	int r = MIN(sz, bs->cursor - bs->read_cursor);

	memcpy(buffer + bs->read_cursor, buf, r);

	return r;
}

int bstream_bprintf(struct binstream *bs, const char *fmt, ...) {
	char c;
	va_list args;

	va_start(args, fmt);

	for (c = *fmt; c; c = *++fmt) {
		switch (c) {
			case 'u': {
				uint32_t i = htonl(va_arg(args, uint32_t));
				bstream_write(bs, &i, sizeof(i));
			} break;
			case 'i': {
				int32_t i = htonl(va_arg(args, int32_t));
				bstream_write(bs, &i, sizeof(i));
			} break;
			case 'h': {
				switch (c = *++fmt) {
					case 'u': {
						uint16_t i = htons(va_arg(args, uint32_t));
						bstream_write(bs, &i, sizeof(i));
					} break;
					case 'i': {
						int16_t i = htons(va_arg(args, int32_t));
						bstream_write(bs, &i, sizeof(i));
					} break;
					case 'h': {
						switch (c = *++fmt) {
							case 'u': {
								uint8_t i = va_arg(args, uint32_t);
								bstream_write(bs, &i, sizeof(i));
							} break;
							case 'i': {
								int8_t i = va_arg(args, int32_t);
								bstream_write(bs, &i, sizeof(i));
							} break;
						}
					} break;
				}
			} break;
			case 's': {
				char *ptr = va_arg(args, char*);
				bstream_write(bs, ptr, strlen(ptr) + 1);
			} break;
			case 'S': case 'r': {
				char *ptr = va_arg(args, char*);
				size_t sz = va_arg(args, size_t);
				bstream_write(bs, ptr, sz);
			} break;
			case '#': case 'R': {
				char *ptr = va_arg(args, void*);
				uint32_t sz = va_arg(args, uint32_t);
				uint32_t w = htonl(sz);
				bstream_write(bs, &w, sizeof(w));
				bstream_write(bs, ptr, sz);
			} break;
			case 'f': {
				void *(*func)(struct bstream *, void *) = va_arg(args, void *(*)(struct bstream *, void *));
				void **ret = va_arg(args, void **);
				void *arg = va_arg(args, void *);

				if (ret == NULL)
					func(bs, arg);
				else
					*ret = func(bs, arg);
			} break;
		}
	}

	va_end(args);

	return 0;
}

int bstream_bscanf(struct binstream *bs, const char *fmt, ...) {
	return 0;
}
