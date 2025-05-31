#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define INITIAL_CAPACITY 64
typedef enum 
{
    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )
    TOKEN_LAMBDA,   // λ
    TOKEN_DOT,      // .
    TOKEN_IDENT,    // variable name like x, y, etc.
    TOKEN_EOF,
    TOKEN_EOE,      // end of expression
    TOKEN_INVALID
} TokenType;


typedef struct 
{
  TokenType type;
  char value;
} Token;

typedef struct 
{
  Token* tokens;
  int count;
} TokenStream;

Token next_token(const char** line);
TokenStream tokenise(const char* input);
char* token_as_string(TokenType type);

#endif // LEXER_H
