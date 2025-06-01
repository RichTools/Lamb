#include "lexer.h"
#include <assert.h>
#include <string.h>


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
    case TOKEN_DEF:
      return ":=";
    case TOKEN_LAMBDA:
      return "\\";
    case TOKEN_EOF:
      return "EOF";
    case TOKEN_EOE:
      return "EOE";
    case TOKEN_INVALID:
      return "Invalid";
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

  if (c == ':' && (*input)[1] == '=') 
  {
    *input += 2; // move past +=
    return (Token){ .type = TOKEN_DEF, .value = 0 };
  }
 
  // Single-character tokens
  switch (c)
  {
    case '(': 
      (*input)++;
      return (Token){ TOKEN_LPAREN, NULL };
    case ')': 
      (*input)++;
      return (Token){ TOKEN_RPAREN, NULL };
    case '.': 
      (*input)++;
      return (Token){ TOKEN_DOT, NULL };
    case '\\':
      (*input)++;
      return (Token){ TOKEN_LAMBDA, NULL };
    case '\n':
      (*input)++;
      return (Token){ TOKEN_EOE, NULL };
  }

 
  // read identifier
  if (isalpha(c))
  {
    const char* start = *input;
    while (isalnum(**input) || **input == '_') (*input)++;
    int len = *input - start;

    char* ident = malloc(len + 1);
    strncpy(ident, start, len);
    ident[len] = '\0';


    return (Token){ .type = TOKEN_IDENT, .value = ident };
  }
 

  // Unknown Tokens 
  (*input)++;
  return (Token){ .type = TOKEN_INVALID, .value = NULL };
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
      if (tok.value != NULL)
      {
        fprintf(stderr, "Invalid Token: '%s'\n", tok.value);
      }
      else
      {
        fprintf(stderr, "Invalid Token (no value)\n");
      } 
      free(tokens);
      return (TokenStream){0,0};
    }
  }
  printf("Number of Tokens %d\n", count);
  return (TokenStream){tokens, count};
}

void free_token_stream(TokenStream* stream)
{
  if (!stream || !stream->tokens) return;
  
  // Free all token values
  for (int i = 0; i < stream->count; i++) {
    if (stream->tokens[i].value) {
      free(stream->tokens[i].value);
    }
  }
  
  // Free the tokens array
  free(stream->tokens);
  stream->tokens = NULL;
  stream->count = 0;
}
