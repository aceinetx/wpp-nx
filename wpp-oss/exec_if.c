#include "exec.h"
#include <stdio.h>
#include <string.h>

bool
wpp_do_if (wppExec *exec)
{
  wppToken lhs_tok, op_tok, rhs_tok;
  wppObject lhs, rhs;
  bool skip;

  skip = true;

  lhs_tok = wpp_lexer_next (exec->lexer);
  op_tok = wpp_lexer_next (exec->lexer);
  rhs_tok = wpp_lexer_next (exec->lexer);

  lhs.type = (unsigned char)-1;
  rhs.type = (unsigned char)-1;

  if (lhs_tok.type == WPP_TOKEN_IDENTIFIER)
    {
      wppObject *var = wpp_exec_getvar (exec, lhs_tok.as.str);
      if (!var)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "if: undefined variable %s", lhs_tok.as.str);
          return false;
        }
      memcpy (&lhs, var, sizeof (wppObject));
    }
  if (rhs_tok.type == WPP_TOKEN_IDENTIFIER)
    {
      wppObject *var = wpp_exec_getvar (exec, rhs_tok.as.str);
      if (!var)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "if: undefined variable %s", rhs_tok.as.str);
          return false;
        }
      memcpy (&rhs, var, sizeof (wppObject));
    }

  if (lhs_tok.type == WPP_TOKEN_INT)
    {
      lhs.type = WPP_OBJ_INT;
      lhs.as._int = lhs_tok.as.number;
    }
  if (rhs_tok.type == WPP_TOKEN_INT)
    {
      rhs.type = WPP_OBJ_INT;
      rhs.as._int = rhs_tok.as.number;
    }

  if (lhs_tok.type == WPP_TOKEN_FLOAT)
    {
      lhs.type = WPP_OBJ_FLOAT;
      lhs.as._float = lhs_tok.as.fnumber;
    }
  if (rhs_tok.type == WPP_TOKEN_FLOAT)
    {
      rhs.type = WPP_OBJ_FLOAT;
      rhs.as._float = rhs_tok.as.fnumber;
    }

  if (lhs_tok.type == WPP_TOKEN_STRING)
    {
      lhs.type = WPP_OBJ_STRING;
      lhs.as.string = lhs_tok.as.str;
    }
  if (rhs_tok.type == WPP_TOKEN_STRING)
    {
      rhs.type = WPP_OBJ_STRING;
      rhs.as.string = rhs_tok.as.str;
    }

  if (lhs.type == (unsigned char)-1)
    {
      snprintf (exec->error, sizeof (exec->error),
                "if: unsupported type for lhs operand (%d)", lhs_tok.type);
      return false;
    }
  if (rhs.type == (unsigned char)-1)
    {
      snprintf (exec->error, sizeof (exec->error),
                "if: unsupported type for rhs operand (%d)", rhs_tok.type);
      return false;
    }

    /* perform comparisons */
#define DO_CMP(oper)                                                          \
  if (lhs.type == WPP_OBJ_INT)                                                \
    {                                                                         \
      if (rhs.type == WPP_OBJ_INT)                                            \
        skip = !(lhs.as._int oper rhs.as._int);                               \
      if (rhs.type == WPP_OBJ_FLOAT)                                          \
        skip = !(lhs.as._int oper (int) rhs.as._float);                       \
    }                                                                         \
  if (lhs.type == WPP_OBJ_FLOAT)                                              \
    {                                                                         \
      if (rhs.type == WPP_OBJ_INT)                                            \
        skip = !(lhs.as._float oper (float) rhs.as._int);                     \
      if (rhs.type == WPP_OBJ_FLOAT)                                          \
        skip = !(lhs.as._float oper rhs.as._float);                           \
    }

  switch (op_tok.type)
    {
    case WPP_TOKEN_EQEQ:
      skip = !memcmp (&lhs.as, &rhs.as, sizeof (lhs.as));
      if (lhs.type == WPP_OBJ_STRING && rhs.type == WPP_OBJ_STRING)
        {
          skip = !(strcmp (lhs.as.string, rhs.as.string) == 0);
        }
      break;
    case WPP_TOKEN_NOEQ:
      skip = (memcmp (&lhs.as, &rhs.as, sizeof (lhs.as)) < 0) ? 0 : 1;
      if (lhs.type == WPP_OBJ_STRING && rhs.type == WPP_OBJ_STRING)
        {
          skip = !(strcmp (lhs.as.string, rhs.as.string) == 0);
        }
      break;
    case WPP_TOKEN_LESS:
      DO_CMP (<);
      break;
    case WPP_TOKEN_MORE:
      DO_CMP (>);
      /*
            if (lhs.type == WPP_OBJ_INT)
              {
                if (rhs.type == WPP_OBJ_INT)
                  skip = !(lhs.as._int > rhs.as._int);
                if (rhs.type == WPP_OBJ_FLOAT)
                  skip = !(lhs.as._int > (int)rhs.as._float);
              }
            if (lhs.type == WPP_OBJ_FLOAT)
              {
                if (rhs.type == WPP_OBJ_INT)
                  skip = !(lhs.as._float > (float)rhs.as._int);
                if (rhs.type == WPP_OBJ_FLOAT)
                  skip = !(lhs.as._float > rhs.as._float);
              }*/
      break;
    case WPP_TOKEN_LSEQ:
      DO_CMP (<=);
      break;
    case WPP_TOKEN_MREQ:
      DO_CMP (>=);
      break;
    default:
      snprintf (exec->error, sizeof (exec->error),
                "if: invalid token of type %d", op_tok.type);
      return false;
    }
#undef DO_CMP

  if (skip)
    {
      unsigned int if_count = 0;
      for (;;)
        {
          wppToken token = wpp_lexer_next (exec->lexer);
          if (token.type == WPP_TOKEN_IF)
            {
              if_count++;
            }
          else if (token.type == WPP_TOKEN_END)
            {
              if (if_count > 0)
                if_count--;
              else
                {
                  break;
                }
            }
          else if (token.type == WPP_TOKEN_ELSE && if_count == 0)
            {
              break;
            }
        }
    }

  return true;
}

bool
wpp_do_end (wppExec *exec)
{
  (void)exec;
  return true;
}

bool
wpp_do_else (wppExec *exec)
{
  unsigned int if_count = 0;
  for (;;)
    {
      wppToken token = wpp_lexer_next (exec->lexer);
      if (token.type == WPP_TOKEN_IF)
        {
          if_count++;
        }
      else if (token.type == WPP_TOKEN_END)
        {
          if (if_count > 0)
            if_count--;
          else
            {
              break;
            }
        }
    }

  return true;
}
