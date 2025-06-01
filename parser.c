#include "parser.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define todo(msg) \
    do { \
        fprintf(stderr, "TODO: %s at %s:%d\n", msg, __FILE__, __LINE__); \
        assert(0); \
    } while (0)

void print_expr(const Expr* expr)
{
    if (!expr) return;

    switch (expr->type) {
        case EXPR_VAR:
            printf("%s", expr->var.name);
            break;
        case EXPR_ABS:
            printf("(λ%s.", expr->abs.param);
            print_expr(expr->abs.body);
            printf(")");
            break;
        case EXPR_DEF:
            printf("%s := ", expr->def.name);
            print_expr(expr->def.value);
            break;
        case EXPR_APP:
            printf("(");
            print_expr(expr->app.func);
            printf(" ");
            print_expr(expr->app.arg);
            printf(")");
            break;
    }
}

void print_indent(int count, char ch, const char* string, char* value)
{
    putchar('|');
    for (int i = 0; i < count; i++) putchar(ch);
    printf("%s: %s\n",string, value);
}

void print_expr_debug(const Expr* expr, int indent)
{
    if (!expr)
    {
        printf("%*s(null)\n", indent, "");
        return;
    }

    switch (expr->type)
    {
        case EXPR_VAR:
            print_indent(indent, '-' ,"VAR", expr->var.name);
            break;

        case EXPR_ABS:
            print_indent(indent, '-', "ABS λ", expr->abs.param);
            print_expr_debug(expr->abs.body, indent + 2);
            break;

        case EXPR_APP:
            printf("|%*sAPP:\n", indent, "");
            print_expr_debug(expr->app.func, indent + 2);
            print_expr_debug(expr->app.arg, indent + 2);
            break;

        case EXPR_DEF:
            printf("|%*sDEF:\n", indent, "");
            printf("|%*sname: %s\n", indent + 2, "", expr->def.name);
            print_expr_debug(expr->def.value, indent + 2);
            break;
    }
}

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
    e->var.name = strdup(tok.value);
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
  e->abs.param = strdup(param.value);
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
  expect_and_consume(tokens[*pos], TOKEN_RPAREN, pos);

  Expr* e = malloc(sizeof(Expr));
  e->type = EXPR_APP;
  e->app.func = func;
  e->app.arg = arg;
  return e;
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
      if (!inner) return NULL;

      if (tokens[*pos].type != TOKEN_RPAREN)
      {
          fprintf(stderr, "Expected ')' at position %d\n", *pos);
          return NULL;
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
    return NULL;
  }
  (*pos)++;

  Expr* value = parse_expression(tokens, pos);
  if (!value) 
  {
    free(name);
    return NULL;
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
  if (!expr) return NULL;

  while (1)
  {
    int savePos = *pos;
    Expr* next = parse_primary(tokens, pos);
    if (!next) 
    {
      *pos = savePos;
      break;
    }

    // Create an application node
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


