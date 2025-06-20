#include "exec.h"
#include "wpp.h"
#include <stdio.h>

bool
wpp_do_cp (wppExec *exec)
{
  wppToken dest, src;
  wppObject *osrc;

  dest = wpp_lexer_next (exec->lexer);
  src = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (dest, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (src, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  osrc = wpp_exec_getvar (exec, src.as.str);
  if (!osrc)
    {
      snprintf (exec->error, sizeof (exec->error),
                "copy: undefined variable %s", src.as.str);
      return false;
    }

  wpp_exec_assign (exec, dest.as.str, *osrc);

  return true;
}
