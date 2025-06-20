#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

typedef struct
{
  void **data;
  uint32_t size;
  uint32_t length;
} wppArena;

wppArena wpp_arena_new (void);

void *wpp_arena_alloc (wppArena *arena, uint32_t size);
void wpp_arena_append (wppArena *arena, void *ptr);

void wpp_arena_free (wppArena *arena);

#endif
