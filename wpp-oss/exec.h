#ifndef EXEC_H
#define EXEC_H
#include "cfwd.h"
#include "lexer.h"
#include "object.h"
#include "wpp.h"
#include <stddef.h>

#define DO_TEST_TOKEN(v, t)                                                   \
  if (v.type != t)                                                            \
    {                                                                         \
      snprintf (exec->error, sizeof (exec->error), "syntax error");           \
      return false;                                                           \
    }

enum
{
  FCALL_FAIL,
  FCALL_SUCCESS
};

enum
{
  PUSH_FAIL,
  PUSH_SUCCESS
};

typedef struct
{
  char error[256];
  wppObject **vars;
  unsigned int vars_len;
  unsigned int vars_capacity;

  wppArena objects_arena;
  wppArena strings_arena;

  wppLexer *lexer;

  unsigned int ret_stack[256];
  size_t ret_stack_top;

  unsigned int loop_stack[256];
  size_t loop_stack_top;

  unsigned int exit_level;

  FILE *fprint;

  char *currentNamespace;
} wppExec;

wppExec *wpp_exec_new (wppLexer *lexer);
void wpp_exec_free (wppExec *exec);

int wpp_exec_fcall (wppExec *exec, const char *name);
wppObject *wpp_exec_getvar (wppExec *exec, const char *name);
wppObject *wpp_exec_assign (wppExec *exec, const char *name, wppObject object);

int wpp_exec_push_ret_stack (wppExec *exec, unsigned int value);
unsigned int wpp_exec_pop_ret_stack (wppExec *exec);

void wpp_exec_run (wppExec *exec);

bool wpp_exec_obj_eq (wppExec *exec, wppObject *obj, wppObject *other);
bool wpp_exec_obj_add (wppExec *exec, wppObject *obj, wppObject *other);
bool wpp_exec_obj_sub (wppExec *exec, wppObject *obj, wppObject *other);
bool wpp_exec_obj_mul (wppExec *exec, wppObject *obj, wppObject *other);
bool wpp_exec_obj_div (wppExec *exec, wppObject *obj, wppObject *other);

bool wpp_exec_obj_cast (wppExec *exec, wppObject *obj, unsigned char type);

#endif
