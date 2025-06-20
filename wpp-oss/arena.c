#include "arena.h"
#include <stdlib.h>

wppArena
wpp_arena_new (void)
{
  wppArena arena;
  arena.data = NULL;
  arena.length = 0;
  arena.size = 0;
  return arena;
}

void *
wpp_arena_alloc (wppArena *arena, uint32_t size)
{
  arena->data = realloc (arena->data, arena->size + sizeof (void *));
  arena->data[arena->length] = malloc (size);

  arena->length++;
  arena->size = arena->length * sizeof (void *);

  return arena->data[arena->length - 1];
}

void
wpp_arena_append (wppArena *arena, void *ptr)
{
  uint32_t i;
  for (i = 0; i < arena->length; i++)
    {
      if (arena->data[i] == ptr)
        {
          return; /* skip adding duplicate pointer */
        }
    }

  arena->data = realloc (arena->data, arena->size + sizeof (void *));
  arena->data[arena->length] = ptr;
  arena->length++;
  arena->size = arena->length * sizeof (void *);
}

void
wpp_arena_free (wppArena *arena)
{
  uint32_t i;

  if (!arena->data)
    return;

  for (i = 0; i < arena->length; i++)
    {
      free (arena->data[i]);
    }
  free (arena->data);
  arena->data = NULL;
  arena->length = 0;
  arena->size = 0;
}
