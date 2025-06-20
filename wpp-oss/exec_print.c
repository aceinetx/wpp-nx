#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void wpp_printf_handler(const char*, ...);

bool
wpp_do_print (wppExec *exec)
{
  wppToken string;
  char *c, *varname, *varname_c;

  varname = NULL;

  string = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (string, WPP_TOKEN_STRING);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  /* do we even need to allocate varname? */
  c = string.as.str;
  while (*c)
    {
      if (*c == '$')
        {
          varname = malloc (strlen (string.as.str));
          break;
        }
      c++;
    }

  c = string.as.str;
  while (*c)
    {
      if (*c == '$')
        { /* print variable */
          wppObject *var;
          c++;

          varname_c = varname;
          memset (varname, 0, strlen (string.as.str));
          while (*c != '^' && *c)
            {
              *varname_c = *c;
              varname_c++;
              c++;
            }

          var = wpp_exec_getvar (exec, varname);
          if (var)
            {
              switch (var->type)
                {
                case WPP_OBJ_INT:
                  wpp_printf_handler ("%d", var->as._int);
                  break;
                case WPP_OBJ_FLOAT:
                  wpp_printf_handler ("%f", var->as._float);
                  break;
                case WPP_OBJ_STRING:
                  wpp_printf_handler ("%s", var->as.string);
                  break;
                case WPP_OBJ_FUNCTION:
                  wpp_printf_handler ("<function %s at %p>", varname, (void *)var);
                  break;
                default:
                  wpp_printf_handler ("<%s of type %d>", varname, var->type);
                  break;
                }
            }
          else
            {
              wpp_printf_handler ("<unknown var: %s>", varname);
            }
        }
      else
        /* print normal character */
        {
          wpp_printf_handler ("%c", *c);
        }
      c++;
    }

  if (varname)
    free (varname);

  return true;
}

bool
wpp_do_println (wppExec *exec)
{
  wpp_do_print (exec);

  wpp_printf_handler ("\n");
  return true;
}
