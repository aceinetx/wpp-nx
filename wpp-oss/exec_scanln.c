#include "exec.h"
#include "object.h"
#include "wpp.h"
#include <stdio.h>
#include <string.h>

char* wpp_input_handler();

bool
wpp_do_scanln (wppExec *exec)
{
  wppToken dest;
  wppObject src;

  dest = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (dest, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  {
    char *buffer;
    buffer = wpp_input_handler();
    wpp_arena_append(&exec->strings_arena, buffer);

    src.type = WPP_OBJ_STRING;
    src.as.string = buffer;
  }

  wpp_exec_assign (exec, dest.as.str, src);

  return true;
}
