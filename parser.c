#include "parser.h"
#include "lexer.h"
#include <stdio.h>

void expect_and_consume(Token token, TokenType expect, int* pos)
{
  if (token.type != expect) 
  {
    fprintf(stderr, 
            "Expected `%s` but found `%s`, at position %d\n", 
            token_as_string(expect), token_as_string(token.type), *pos);
    exit(1);
  }

  (*pos)++;
}

Token expect_and_get(Token token, TokenType expect, int* pos)
{
  if (token.type != expect) 
  {
    fprintf(stderr, "Expected `%s` but found `%s` at position %d\n", 
            token_as_string(expect), token_as_string(token.type), *pos);
    exit(1);
  }
  (*pos)++;
  return token;
}

Expr* parse_variable(TokenStream tokens, int* pos) 
{  
    Token tok = tokens.tokens[*pos];
    
    expect_and_consume(tok, TOKEN_IDENT, pos);
    Expr* e = malloc(sizeof(Expr));
    e->type = EXPR_VAR;
    e->var.name = tok.value;
    return e;
}

Expr* parse_function(TokenStream tokenStream, int* pos)
{
  Token* tokens = tokenStream.tokens;

  expect_and_consume(tokens[*pos], TOKEN_LPAREN, pos);
  expect_and_consume(tokens[*pos], TOKEN_LAMBDA, pos);
  const Token param = expect_and_get(tokens[*pos], TOKEN_IDENT, pos);
  expect_and_consume(tokens[*pos], TOKEN_DOT, pos); 

  Expr* body = parse_expression(tokenStream, pos);
  if (!body) return NULL;

  expect_and_consume(tokens[*pos], TOKEN_RPAREN, pos);
  Expr* e = malloc(sizeof(Expr));
  e->type = EXPR_ABS;
  e->abs.param = param.value;
  e->abs.body = body;
  return e;
}

Expr* parse_application(TokenStream tokenStream, int* pos)
{
  // ( expr expr )
  Token* tokens = tokenStream.tokens;
  expect_and_consume(tokens[*pos], TOKEN_LPAREN, pos);

  Expr* func = parse_expression(tokenStream, pos);
  if (!func) return NULL;

  Expr* arg = parse_expression(tokenStream, pos);
  if (!arg)
  {
    free(func);
    return NULL;
  }
  expect_and_consume(tokens[*pos], TOKEN_LPAREN, pos);

  Expr* e = malloc(sizeof(Expr));
  e->type = EXPR_APP;
  e->app.func = func;
  e->app.arg = arg;
  return e;
}


Expr* parse_expression(TokenStream tokens, int* pos)
{
  Token tok = tokens.tokens[*pos];
  while (*pos < tokens.count) 
  {
    if (tok.type == TOKEN_IDENT)
    {
      printf("Parse Var\n");
      return parse_variable(tokens, pos);
    }
    else if (tok.type == TOKEN_LPAREN)
    {
      // Peek ahead
      int savePos = *pos;
      if (tokens.tokens[savePos + 1].type == TOKEN_LAMBDA)
      {
        printf("Parse Func\n");
        return parse_function(tokens, pos);
      }
      else
      {
        return parse_application(tokens, pos);
      }
    }
    tok = tokens.tokens[(*pos)++];
  }

  fprintf(stderr, "Unexpected token at position %d\n", *pos);
  return 0;
  
  //printf("Expr: %s\n", expr);
}


