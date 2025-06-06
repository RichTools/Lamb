#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum
{
  EXPR_VAR,   // <name>
  EXPR_ABS,   // <function>
  EXPR_APP,   // <application>
  EXPR_DEF,   // <assignment>
  EXPR_IMPORT // <import>
} ExprType;

typedef struct Expr Expr;

// Variable (name)
typedef struct
{
  char* name;
} Var;

// Abstraction (function)
typedef struct
{
  char* param;
  Expr* body;
} Abs;

// Application 
typedef struct
{
  Expr* func;
  Expr* arg;
} App;

typedef struct 
{
  char* name;
  Expr* value;
} Def;

typedef struct
{
    const char* filename;
} ImportExpr;

struct Expr 
{
  ExprType type;
  union
  {
      Var var;
      Abs abs;
      App app;
      Def def;
      ImportExpr impt;
  };
};

Expr* parse_variable(TokenStream tokens, int* pos);
Expr* parse_function(TokenStream tokens, int* pos);
Expr* parse_expression(TokenStream tokens, int* pos);
Expr* parse_import(TokenStream tokens, int* pos);

void free_expr(Expr* e);

#endif // PARSER_H
