#include "lexer.h"
#include <assert.h>

char* token_as_string(TokenType type)
{
  switch (type)
  {
    case TOKEN_LPAREN:
      return "(";
    case TOKEN_DOT:
      return ".";
    case TOKEN_IDENT:
      return "identifier";
    case TOKEN_RPAREN:
      return ")";
    case TOKEN_LAMBDA:
      return "\\";
    case TOKEN_EOF:
      return "EOF";
    default:
      assert(0 && "Unreachable");
      
  }
}

Token next_token(const char** input)
{
  if (input == NULL || *input == NULL) {
    fprintf(stderr, "Error: Invalid input pointer\n");
    return (Token){ TOKEN_EOF, 0 };
  }

  while (**input == ' ' || **input == '\t' || **input == '\n')
  {
    (*input)++; // skip whitespace
  }
  
  char c = **input;

  if (c == '\0') 
  {
    return (Token){ TOKEN_EOF, 0} ;
  }
  
  (*input)++;

  switch (c)
  {
    case '(':
      return (Token){ .type = TOKEN_LPAREN, .value = c };
    case ')':
      return (Token){ .type = TOKEN_RPAREN, .value = c };
    case '.':
      return (Token){ .type = TOKEN_DOT, .value = c };
    case '\\':
      return (Token){ .type = TOKEN_LAMBDA, .value = c };
    case '\n':
      return (Token){ .type = TOKEN_EOE, .value = c};
    default:
      if (isalpha(c))
      {
        return (Token){ .type = TOKEN_IDENT, .value = c };
      }
      else
      {
        return (Token){ .type = TOKEN_INVALID, .value = c };
      }
  }
}

TokenStream tokenise(const char* input)
{
  const char* cursor = input;
  int capacity = INITIAL_CAPACITY;
  int size = 0;
  int count = 0;

  Token* tokens = malloc(capacity * sizeof(Token));
  if (!tokens) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  while (1)
  {
    Token tok = next_token(&cursor);

    // Grow array if needed 
    if (size >= capacity)
    {
      capacity *= 2;
      tokens = realloc(tokens, capacity * sizeof(Token));
      if (!tokens)
      {
        fprintf(stderr, "Memory reallocation failed\n");
        exit(1);
      }
    }

    tokens[size++] = tok;
    count++;

    if (tok.type == TOKEN_EOF) break;

    if (tok.type == TOKEN_INVALID)
    {
      fprintf(stderr, "Invalid Token: '%c'\n", tok.value);
      free(tokens);
      return (TokenStream){0,0};
    }
  }
  printf("Number of Tokens %d\n", count);
  return (TokenStream){tokens, count};
}
