#include "lexer.h"
#include "cfwd.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool
is_letter (char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static bool
is_identifier_letter (char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static bool
is_digit (char c)
{
  return (c >= '0' && c <= '9');
}

static wppToken
wpp_token_new (void)
{
  wppToken token;
  token.line = 0;
  token.as.str = NULL;
  token.type = WPP_TOKEN_NULL;
  return token;
}

wppLexer *
wpp_lexer_new (char *code)
{
  wppLexer *lexer = malloc (sizeof (wppLexer));
  lexer->strings_arena = wpp_arena_new ();
  lexer->code = code;
  lexer->code_len = strlen (lexer->code);
  lexer->line = 1;
  lexer->pos = 0;
  return lexer;
}

void
wpp_lexer_free (wppLexer *lexer)
{
  wpp_arena_free (&lexer->strings_arena);
  free (lexer);
}

/* this doesn't append to strings arena: you'll have to do it manually */
static wppToken
wpp_lexer_identifier (wppLexer *lexer)
{
  wppToken tok;
  size_t len;

  tok = wpp_token_new ();
  tok.type = WPP_TOKEN_IDENTIFIER;
  tok.line = lexer->line;
  tok.as.str = malloc (1);

  len = 1;
  while (lexer->pos < lexer->code_len)
    {
      char c = lexer->code[lexer->pos];

      if (!is_identifier_letter (c) && !is_digit (c) && !(c == ':'))
        {
          break;
        }

      tok.as.str = realloc (tok.as.str, len + 1);
      tok.as.str[len - 1] = c;
      tok.as.str[len] = '\0';
      len++;

      lexer->pos++;
    }

  return tok;
}

static wppToken
wpp_lexer_string (wppLexer *lexer)
{
  wppToken tok;
  size_t len;
  bool definition;

  tok = wpp_token_new ();
  tok.type = WPP_TOKEN_STRING;
  tok.line = lexer->line;
  tok.as.str = malloc (1);

  len = 1;

  definition = false;
  while (lexer->pos < lexer->code_len)
    {
      char c = lexer->code[lexer->pos];

      if (c == '"')
        {
          if (definition)
            {
              lexer->pos++;
              break;
            }
          definition = true;
          lexer->pos++;
          continue;
        }

      if (definition)
        {
          tok.as.str = realloc (tok.as.str, len + 1);
          tok.as.str[len - 1] = c;
          tok.as.str[len] = '\0';
          len++;
        }

      lexer->pos++;
    }

  wpp_arena_append (&lexer->strings_arena, tok.as.str);

  return tok;
}

static int
hex_char_to_value (char c)
{
  if (c >= '0' && c <= '9')
    {
      return c - '0';
    }
  else if (c >= 'a' && c <= 'f')
    {
      return 10 + (c - 'a');
    }
  else if (c >= 'A' && c <= 'F')
    {
      return 10 + (c - 'A');
    }
  return -1; /* invalid character */
}

static wppToken
wpp_lexer_number (wppLexer *lexer)
{
  wppToken tok;
  bool is_hex;
  bool is_float = false;
  char c;
  int value;
  int start_pos = lexer->pos;

  tok = wpp_token_new ();
  tok.type = WPP_TOKEN_INT;
  tok.line = lexer->line;

  is_hex = false;
  tok.as.number = 0;

  while (lexer->pos < lexer->code_len)
    {
      c = lexer->code[lexer->pos];

      if (!is_digit (c) && !is_hex)
        {
          if (c == 'x')
            {
              is_hex = true;
              lexer->pos++;
              continue;
            }
          else if (c == '.')
            {
              is_float = true;
              break;
            }
          else
            {
              break; /* invalid character for number */
            }
        }

      if (is_hex)
        {
          value = hex_char_to_value (c);
          if (value == -1)
            {
              break; /* invalid hex character */
            }
          tok.as.number = (tok.as.number * 16) + value;
        }
      else
        {
          if (!is_digit (c))
            {
              break; /* Invalid character for decimal number */
            }
          tok.as.number = (tok.as.number * 10) + (c - '0');
        }
      lexer->pos++;
    }

  if (is_float)
    {
      int float_start, float_end, float_len;
      char float_buf[64];

      /* parse the float from the original string */
      float_start = start_pos;
      float_end = lexer->pos;
      /* find the end of the float literal */
      while (lexer->pos < lexer->code_len
             && (is_digit (lexer->code[lexer->pos])
                 || lexer->code[lexer->pos] == '.'))
        lexer->pos++;
      float_end = lexer->pos;
      float_len = float_end - float_start;
      if (float_len >= (int)sizeof (float_buf))
        float_len = sizeof (float_buf) - 1;
      memcpy (float_buf, &lexer->code[float_start], float_len);
      float_buf[float_len] = '\0';
      tok.type = WPP_TOKEN_FLOAT;
      tok.as.fnumber = strtof (float_buf, NULL);
    }

  return tok;
}

wppToken
wpp_lexer_next (wppLexer *lexer)
{
  wppToken token = wpp_token_new ();

  while (lexer->pos < lexer->code_len)
    {
      char c = lexer->code[lexer->pos];
      token = wpp_token_new ();

      if (c == '\n')
        lexer->line++;

      if (is_letter (c))
        {
          token = wpp_lexer_identifier (lexer);
#define DO_IDENTIFIR(s, v)                                                    \
  if (strcmp (token.as.str, s) == 0)                                          \
    token.type = v;
          DO_IDENTIFIR ("fn", WPP_TOKEN_FN);
          DO_IDENTIFIR ("nf", WPP_TOKEN_NF);
          DO_IDENTIFIR ("print", WPP_TOKEN_PRINT);
          DO_IDENTIFIR ("println", WPP_TOKEN_PRINTLN);
          DO_IDENTIFIR ("call", WPP_TOKEN_CALL);
          DO_IDENTIFIR ("var", WPP_TOKEN_VAR);
          DO_IDENTIFIR ("cp", WPP_TOKEN_CP);
          DO_IDENTIFIR ("scanln", WPP_TOKEN_SCANLN);
          DO_IDENTIFIR ("return", WPP_TOKEN_RETURN);
          DO_IDENTIFIR ("cast", WPP_TOKEN_CAST);
          DO_IDENTIFIR ("namespace", WPP_TOKEN_NAMESPACE);
          DO_IDENTIFIR ("nsEnd", WPP_TOKEN_NSEND);
          DO_IDENTIFIR ("if", WPP_TOKEN_IF);
          DO_IDENTIFIR ("else", WPP_TOKEN_ELSE);
          DO_IDENTIFIR ("end", WPP_TOKEN_END);
          DO_IDENTIFIR ("loop", WPP_TOKEN_LOOP);
          DO_IDENTIFIR ("lend", WPP_TOKEN_LEND);
          DO_IDENTIFIR ("break", WPP_TOKEN_BREAK);
#undef DO_IDENTIFIR
          if (token.type == WPP_TOKEN_IDENTIFIER)
            wpp_arena_append (&lexer->strings_arena, token.as.str);
          else
            free (token.as.str);
        }
      else if (is_digit (c))
        {
          token = wpp_lexer_number (lexer);
        }
      else if (c == '"')
        {
          token = wpp_lexer_string (lexer);
        }
      else if (c == ';')
        {
          token.type = WPP_TOKEN_SEMICOLON;
          lexer->pos++;
        }
      else if (c == '=')
        {
          token.type = WPP_TOKEN_EQ;
          if (lexer->code[lexer->pos + 1] == '=')
            {
              token.type = WPP_TOKEN_EQEQ;
              lexer->pos++;
            }
          lexer->pos++;
        }
      else if (c == '+')
        {
          token.type = WPP_TOKEN_ADD;
          lexer->pos++;
        }
      else if (c == '-')
        {
          token.type = WPP_TOKEN_SUB;
          lexer->pos++;
        }
      else if (c == '*')
        {
          token.type = WPP_TOKEN_MUL;
          lexer->pos++;
        }
      else if (c == '/')
        {
          token.type = WPP_TOKEN_DIV;
          lexer->pos++;
        }
      else if (c == '?')
        {
          /* comment */
          while (!(lexer->code[lexer->pos] == '-'
                   && lexer->code[lexer->pos + 1] == '?'))
            lexer->pos++;
          lexer->pos++;
        }
      else if (c == '!' && lexer->code[lexer->pos + 1] == '=')
        {
          token.type = WPP_TOKEN_NOEQ;
          lexer->pos += 2;
        }
      else if (c == '<' && lexer->code[lexer->pos + 1] == '=')
        {
          token.type = WPP_TOKEN_LSEQ;
          lexer->pos += 2;
        }
      else if (c == '>' && lexer->code[lexer->pos + 1] == '=')
        {
          token.type = WPP_TOKEN_MREQ;
          lexer->pos += 2;
        }
      else if (c == '<')
        {
          token.type = WPP_TOKEN_LESS;
          lexer->pos++;
        }
      else if (c == '>')
        {
          token.type = WPP_TOKEN_MORE;
          lexer->pos++;
        }

      if (token.type != WPP_TOKEN_NULL)
        {
          return token;
        }

      lexer->pos++;
    }

  token.type = WPP_TOKEN_FLEND;
  token.line = lexer->line;
  return token;
}
