#include "exec.h"
#include <stdio.h>

bool
wpp_do_namespace (wppExec *exec)
{
  wppToken arg;

  arg = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (arg, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  exec->currentNamespace = arg.as.str;

  return true;
}

bool
wpp_do_nsend (wppExec *exec)
{
  if (!exec->currentNamespace)
    {
      snprintf (exec->error, sizeof (exec->error),
                "nsend: not in a namespace");
      return false;
    }

  exec->currentNamespace = NULL;
  return true;
}
