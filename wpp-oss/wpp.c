#include "exec.h"
#include "lexer.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char **argv)
{
  wppLexer *lexer;
  wppExec *exec;
  FILE *file;
  int i, exit_level;
  struct stat stat;
  char *code;
  char *filename = NULL;

  exit_level = 0;

  if (argc <= 1)
    {
      usage ();
      return 1;
    }

  for (i = 1; i < argc; i++)
    {
      char *arg = argv[i];
      if (arg[0] == '-')
        {
          if (!strcmp (arg, "--help"))
            {
              usage ();
              printf ("sizeof(wppLexer) = %u\n",
                      (unsigned int)sizeof (wppLexer));
              printf ("sizeof(wppExec) = %u\n",
                      (unsigned int)sizeof (wppExec));
              printf ("sizeof(wppObject) = %u\n",
                      (unsigned int)sizeof (wppObject));
              printf ("sizeof(wppToken) = %u\n",
                      (unsigned int)sizeof (wppToken));
              return 0;
            }
          else
            {
              printf ("invalid argument: %s", arg);
              return 1;
            }
        }
      else
        filename = arg;
    }

  if (!filename)
    {
      puts ("wpp: no filename provided");
      return 1;
    }

  /* open the file */
  file = fopen (filename, "rb");
  if (!file)
    {
      perror ("wpp: fopen");
      return 1;
    }

  /* get file size */
  if (fstat (fileno (file), &stat) == -1)
    {
      perror ("wpp: fstat");
      fclose (file);
      return 1;
    }

  if (!stat.st_size)
    {
      puts ("wpp: stat.st_size: file is empty");
      fclose (file);
      return 1;
    }

  /* allocate memory for file contents */
  code = (char *)malloc (stat.st_size);
  if (!code)
    {
      perror ("wpp: malloc");
      fclose (file);
      return 1;
    }

  /* read entire file */
  if (fread (code, 1, stat.st_size, file) != (size_t)stat.st_size)
    {
      perror ("wpp: fread");
      free (code);
      fclose (file);
      return 1;
    }

  fclose (file);

  {
    lexer = wpp_lexer_new (code);
    exec = wpp_exec_new (lexer);

    wpp_exec_run (exec);
    if (*exec->error)
      {
        printf ("wpp: %s\n", exec->error);
        exit_level = 1;
      }
    else
      {
        exit_level = exec->exit_level;
      }

    wpp_lexer_free (lexer);
    wpp_exec_free (exec);
  }

  /* free up the file buffer */
  free (code);
  return exit_level;
}

static void
usage (void)
{
  puts ("usage: wpp [filename] [options]...");
}
