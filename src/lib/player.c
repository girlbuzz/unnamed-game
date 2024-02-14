#define _XOPEN_SOURCE 500

#include <player.h>
#include <binstream.h>

void *serialize_player(struct binstream *bs, const struct player *player) {
    bstream_bprintf(bs, "");
    return NULL;
}
