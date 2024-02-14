#ifndef __BINSTREAM_H
#define __BINSTREAM_H

#include <stddef.h>

#ifdef _XOPEN_SOURCE
#include <unistd.h>
#else
typedef long off_t;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

struct binstream {
	void *buffer;
	size_t capacity;
	off_t cursor, read_cursor;
};

#define BINSTREAM_GET_CAPACITY(bs) (((bs)->capacity) & ~(1 << (sizeof((bs)->capacity) * 8) - 1))
#define BINSTREAM_SELF_ALLOCATED(bs) (((bs)->capacity) & (1 << (sizeof((bs)->capacity) * 8) - 1))

/** create a binary stream
 * @param bs the stream to initialize
 * @param capacity size of the buffer, if < 1 will be resized when full
 */
void bstream_init(struct binstream *bs, size_t capacity);

/** create a binary stream, using buffer as the underlying buffer
 * @param bs the stream to initialize
 * @param capacity size of the buffer
 */
void bstream_with_buf(struct binstream *bs, void *buffer, size_t capacity);

/** frees a binary stream
 * @param bs the stream to free
 */
void bstream_free(struct binstream *bs);

/** reset the cursor to the beginning
 * @param bs the stream
 */
void bstream_rewind(struct binstream *bs);

/** reposition read/write buffer offset
 * @param bs the stream
 * @param offset from whence
 * @param whence one of SEEK_SET, SEEK_CUR, SEEK_END
 */
off_t bstream_lseek(struct binstream *bs, off_t offset, int whence);

/** write data to a bstream
 * @param bs the stream
 * @param buf the data
 * @param sz size
 *
 * @return the number of bytes written, or a negative number on error.
 */
int bstream_write(struct binstream *bs, const void *buf, size_t sz);

/** read data from a bstream
 * @param bs the stream
 * @param buf the buffer
 * @param sz size
 *
 * @return the number of bytes read, or a negative number on error.
 */
int bstream_read(struct binstream *bs, void *buf, size_t sz);

/** write data to a bstream with binary formating
 * @param bs the stream
 * @param fmt the format string
 *
 * @return the number of bytes written, or negative number on error.
 *
 * @details
 * {hh,h}u: {8,16}32-bit unsigned integer
 *
 * {hh,h}i: {8,16}32-bit signed integer
 *
 * s: null terminated string. the null terminator will be written.
 *
 * S: string with len passed as a second argument. the size isn't written.
 *
 * r: functionally the same as S. meant mainly for raw data instead of strings.
 * {hh,h}#: string with len passed as a second argument.
 *   the size is written as a {8,16}32-bit integer before the data.
 *
 * {hh,h}R: binary data with len passed as a second argument.
 *  the size is written as a {8,16}32-bit integer before the data.
 *
 * f: use a custom serialization function of type void *(*)(void*).
 *
 * @example
 * ```c
 * struct binstream bs;
 * long ret;
 * char data[32] = "hello";
 *
 * bstream_init(&bs, 512);
 *
 * // use the `s_string` function to serialize `data`. return value is in `ret`.
 * bstream_bprintf(&bs, "f", s_string, &ret, data);
 *
 * // write a 32-bit signed integer, then a 16-bit unsigned integer.
 * // then a null terminated string
 * bstream_bprintf(&bs, "ihus", -5, 200, "hello!");
 * ```
 *
 *  all integers are written as network endian (big endian)
 */
int bstream_bprintf(struct binstream *bs, const char *fmt, ...);

/** read data from a bstream with binary formatting
 * @param bs the stream
 * @param fmt the format string
 *
 * @return the number of bytes read, or negative number on error.
 */
int bstream_bscanf(struct binstream *bs, const char *fmt, ...);

#endif /* __BINSTREAM_H */
