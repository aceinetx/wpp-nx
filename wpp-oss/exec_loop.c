#include "exec.h"
#include "wpp.h"

static int
wpp_exec_push_loop_stack (wppExec *exec, unsigned int value)
{
  if (exec->loop_stack_top + 1
      >= sizeof (exec->loop_stack) / sizeof (unsigned int))
    {
      snprintf (exec->error, sizeof (exec->error), "loop stack overflow");
      return PUSH_FAIL;
    }
  exec->loop_stack[++(exec->loop_stack_top)] = value;
  return PUSH_SUCCESS;
}

static unsigned int
wpp_exec_pop_loop_stack (wppExec *exec)
{
  if (exec->loop_stack_top == 0)
    {
      snprintf (exec->error, sizeof (exec->error), "loop stack underflow");
      return 0;
    }
  return exec->loop_stack[(exec->loop_stack_top)--];
}

bool
wpp_do_loop (wppExec *exec)
{
  wpp_exec_push_loop_stack (exec, exec->lexer->pos);
  return true;
}

bool
wpp_do_lend (wppExec *exec)
{
  exec->lexer->pos = wpp_exec_pop_loop_stack (exec);
  wpp_exec_push_loop_stack (exec, exec->lexer->pos);
  return true;
}

bool
wpp_do_break (wppExec *exec)
{
  unsigned int loop_index = 0;
  for (;;)
    {
      wppToken token = wpp_lexer_next (exec->lexer);

      if (token.type == WPP_TOKEN_LOOP)
        {
          loop_index++;
        }
      else if (token.type == WPP_TOKEN_LEND)
        {
          if (loop_index > 0)
            loop_index--;
          else
            break;
        }
    }
  return true;
}
