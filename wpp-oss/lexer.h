#ifndef LEXER_H
#define LEXER_H

#include "arena.h"
#include <stdint.h>

enum wppTokenType
{
  WPP_TOKEN_NULL = 0,
  WPP_TOKEN_FLEND,

  WPP_TOKEN_INT,
  WPP_TOKEN_FLOAT,

  WPP_TOKEN_IDENTIFIER,
  WPP_TOKEN_STRING,

  WPP_TOKEN_FN,
  WPP_TOKEN_NF,

  WPP_TOKEN_PRINT,
  WPP_TOKEN_PRINTLN,
  WPP_TOKEN_SCANLN,
  WPP_TOKEN_RETURN,
  WPP_TOKEN_CAST,

  WPP_TOKEN_NAMESPACE,
  WPP_TOKEN_NSEND,

  WPP_TOKEN_IF,
  WPP_TOKEN_ELSE,
  WPP_TOKEN_END,

  WPP_TOKEN_CALL,

  WPP_TOKEN_VAR,

  WPP_TOKEN_EQ,
  WPP_TOKEN_ADD,
  WPP_TOKEN_SUB,
  WPP_TOKEN_MUL,
  WPP_TOKEN_DIV,

  WPP_TOKEN_EQEQ,
  WPP_TOKEN_NOEQ,
  WPP_TOKEN_LESS,
  WPP_TOKEN_MORE,
  WPP_TOKEN_LSEQ,
  WPP_TOKEN_MREQ,

  WPP_TOKEN_LOOP,
  WPP_TOKEN_LEND,
  WPP_TOKEN_BREAK,

  WPP_TOKEN_CP,

  WPP_TOKEN_SEMICOLON
};

typedef struct
{
  enum wppTokenType type;

  union
  {
    int number;
    float fnumber;
    char *str;
  } as;

  unsigned int line;
} wppToken;

typedef struct
{
  char *code;
  unsigned int pos;
  unsigned int code_len;

  wppArena strings_arena;

  unsigned int line;
} wppLexer;

wppLexer *wpp_lexer_new (char *code);
void wpp_lexer_free (wppLexer *lexer);

wppToken wpp_lexer_next (wppLexer *lexer);

#endif
