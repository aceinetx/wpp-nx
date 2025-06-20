#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
wpp_exec_obj_cast (wppExec *exec, wppObject *obj, unsigned char type)
{
  /* numbers to numbers */
  if (obj->type == WPP_OBJ_INT && type == WPP_OBJ_FLOAT)
    {
      obj->type = type;
      obj->as._float = (float)obj->as._int;
    }
  else if (obj->type == WPP_OBJ_FLOAT && type == WPP_OBJ_INT)
    {
      obj->type = type;
      obj->as._int = (int)obj->as._float;
    }
  /* strings to numbers */
  else if (obj->type == WPP_OBJ_STRING && type == WPP_OBJ_INT)
    {
      obj->type = type;
      obj->as._int = atoi (obj->as.string);
    }
  else if (obj->type == WPP_OBJ_STRING && type == WPP_OBJ_FLOAT)
    {
      obj->type = type;
      obj->as._float = (float)atof (obj->as.string);
    }
  /* numbers to strings */
  else if (obj->type == WPP_OBJ_INT && type == WPP_OBJ_STRING)
    {
      char *s
          = wpp_arena_alloc (&exec->strings_arena, 21); /* size large enough */
      sprintf (s, "%d", obj->as._int);
      obj->type = type;
      obj->as.string = s;
    }
  else if (obj->type == WPP_OBJ_FLOAT && type == WPP_OBJ_STRING)
    {
      char *s
          = wpp_arena_alloc (&exec->strings_arena, 25); /* size large enough */
      sprintf (s, "%f", obj->as._float);
      obj->type = type;
      obj->as.string = s;
    }
  else
    {
      snprintf (exec->error, sizeof (exec->error), "unsupported cast");
      return false;
    }

  return true;
}

bool
wpp_do_cast (wppExec *exec)
{
  wppToken var_tok, type;
  wppObject *var;
  unsigned char obj_type;

  var_tok = wpp_lexer_next (exec->lexer);
  type = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (var_tok, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (type, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  var = wpp_exec_getvar (exec, var_tok.as.str);
  if (!var)
    {
      snprintf (exec->error, sizeof (exec->error),
                "cast: undefined variable %s", var_tok.as.str);
      return false;
    }

  if (!strcmp (type.as.str, "int"))
    {
      obj_type = WPP_OBJ_INT;
    }
  else if (!strcmp (type.as.str, "float"))
    {
      obj_type = WPP_OBJ_FLOAT;
    }
  else if (!strcmp (type.as.str, "str"))
    {
      obj_type = WPP_OBJ_STRING;
    }
  else
    {
      snprintf (exec->error, sizeof (exec->error),
                "cast: unknown type name %s", type.as.str);
      return false;
    }

  wpp_exec_obj_cast (exec, var, obj_type);

  return true;
}
