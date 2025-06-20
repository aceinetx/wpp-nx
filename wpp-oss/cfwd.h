#ifndef CFWD_H
#define CFWD_H
#include <stddef.h>
#include <stdio.h>

int snprintf (char *s, size_t n, const char *format, ...);
char *strdup (const char *);
float strtof (const char *str, char **endptr);
int fileno (FILE *stream);

#endif
