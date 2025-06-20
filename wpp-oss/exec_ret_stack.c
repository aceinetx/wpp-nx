#include "exec.h"
#include <stdio.h>

int
wpp_exec_push_ret_stack (wppExec *exec, unsigned int value)
{
  if (exec->ret_stack_top + 1
      >= sizeof (exec->ret_stack) / sizeof (unsigned int))
    {
      snprintf (exec->error, sizeof (exec->error), "stack overflow");
      return PUSH_FAIL;
    }
  exec->ret_stack[++(exec->ret_stack_top)] = value;
  return PUSH_SUCCESS;
}

unsigned int
wpp_exec_pop_ret_stack (wppExec *exec)
{
  if (exec->ret_stack_top == 0)
    {
      snprintf (exec->error, sizeof (exec->error), "stack underflow");
      return 0;
    }
  return exec->ret_stack[(exec->ret_stack_top)--];
}
