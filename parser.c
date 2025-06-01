#include "parser.h"
#include "debug.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "diagnostics.h"

void expect_and_consume(Token token, TokenType expect, int* pos)
{
  if (token.type != expect) 
  {
    char buffer[128]; 
    
    snprintf(buffer, sizeof(buffer), 
             "Expected `%s` but found `%s`", 
             token_as_string(expect), 
             token_as_string(token.type)
             );

    report_diag(DIAG_ERROR, *pos, buffer);
  }
  (*pos)++;
}

Token expect_and_get(Token token, TokenType expect, int* pos)
{
  if (token.type != expect) 
  {
    char buffer[128]; 
    
    snprintf(buffer, sizeof(buffer), 
             "Expected `%s` but found `%s`", 
             token_as_string(expect), 
             token_as_string(token.type)
             );

    report_diag(DIAG_ERROR, *pos, buffer);
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
    e->var.name = strdup(tok.value);
    return e;
}

Expr* parse_function(TokenStream tokenStream, int* pos)
{
  Token* tokens = tokenStream.tokens;

  expect_and_consume(tokens[*pos], TOKEN_LPAREN, pos);
  expect_and_consume(tokens[*pos], TOKEN_LAMBDA, pos);
  
  char* params[64]; // Arbirary Limit of 64 parameters TODO: dynamic array
  int param_count = 0;

  while (tokens[*pos].type == TOKEN_IDENT)
  {
    if (param_count >= 64)
    {
      report_diag(DIAG_ERROR, *pos, "Too many parameters in Lambda abstraction.");
    }
    params[param_count++] = strdup(tokens[*pos].value);
    (*pos)++;
  }

  if (param_count == 0)
  {
    report_diag(DIAG_ERROR, *pos, "Invalid Syntax: Lambdas with no parameters are not allowed.");
  }

  expect_and_consume(tokens[*pos], TOKEN_DOT, pos); 

  Expr* body = parse_expression(tokenStream, pos);
  if (!body) 
  {
    report_diag(DIAG_ERROR, *pos, "Invalid Syntax: Empty Function Body not allowed.");
  }

  expect_and_consume(tokens[*pos], TOKEN_RPAREN, pos);

  for (int i = param_count - 1; i >= 0; i--)
  {
    Expr* abs = malloc(sizeof(Expr)); // free this
    abs->type = EXPR_ABS;
    abs->abs.param = params[i];
    abs->abs.body = body;
    body = abs;
  }

  return body;
}


Expr* parse_primary(TokenStream tokenStream, int* pos)
{
  Token tok = tokenStream.tokens[*pos];
  Token* tokens = tokenStream.tokens;

  if (tok.type == TOKEN_IDENT)
  {
    return parse_variable(tokenStream, pos);
  }

  if (tok.type == TOKEN_LPAREN)
  {
    // Could be (expr) or (λx.expr)
    int savePos = *pos;
    if (tokens[savePos + 1].type == TOKEN_LAMBDA)
    {
        return parse_function(tokenStream, pos);
    }
    else
    {
      (*pos)++;  // consume '('
      Expr* inner = parse_expression(tokenStream, pos);
      if (!inner)
      {
        report_diag(DIAG_ERROR, *pos, "Syntax Error: Invalid Expression");
      }

      if (tokens[*pos].type != TOKEN_RPAREN)
      {
        report_diag(DIAG_ERROR, *pos, "Expected `)`");
      }
      (*pos)++;
      return inner;
    }
  }
  return NULL;
}

Expr* parse_definition(TokenStream tokens, int* pos)
{
  if (tokens.tokens[*pos].type != TOKEN_IDENT) return NULL;
  

  char* name = strdup(tokens.tokens[*pos].value);
  (*pos)++;

  if (tokens.tokens[*pos].type != TOKEN_DEF)
  {
    free(name);
    report_diag(DIAG_ERROR, *pos, "Syntax Error: Expected `:=` after identifier in definition");
  }
  (*pos)++;

  Expr* value = parse_expression(tokens, pos);
  if (!value) 
  {
    free(name);
    report_diag(DIAG_ERROR, *pos, "Syntax Error: Invalid expression after `:=` in definition");
  }

  Expr* def = malloc(sizeof(Expr));
  def->type = EXPR_DEF; 
  def->def.name = name;
  def->def.value = value;

  return def;
}



Expr* parse_expression(TokenStream tokens, int* pos)
{
  // check for definition
  int savePos = *pos;

  if (tokens.tokens[savePos].type == TOKEN_IDENT && 
      tokens.tokens[savePos + 1].type == TOKEN_DEF)
  {
    return parse_definition(tokens, pos);
  }

  // parse Primary expr 
  Expr* expr = parse_primary(tokens, pos);
  if (!expr) 
  {
    report_diag(DIAG_ERROR, *pos, "Invalid Syntax: Cannot parse empty Expression.");
  }

  while (1)
  {
    int savePos = *pos;
    Expr* next = parse_primary(tokens, pos);
    if (!next) 
    {
      *pos = savePos;
      break;
    }

    // Create an application node - parse application
    Expr* app = malloc(sizeof(Expr));
    app->type = EXPR_APP;
    app->app.func = expr;
    app->app.arg = next;
    expr = app;  // left associative
  }

  return expr;
}


void free_expr(Expr* e)
{
  if (!e) return;

  switch (e->type) {
    case EXPR_VAR:
      free(e->var.name);
      break;
    case EXPR_DEF:
      free(e->def.name);
      free_expr(e->def.value);
      break;
    case EXPR_ABS:
      free(e->abs.param);
      free_expr(e->abs.body);
      break;
    case EXPR_APP:
      free_expr(e->app.func);
      free_expr(e->app.arg);
      break;
  }

  free(e);
}
