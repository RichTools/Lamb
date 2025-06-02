#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

// Linked List of Entries to the Symbol Table
typedef struct EnvEntry {
    const char* name;
    Expr* value;
    struct EnvEntry* next;
} Env;

typedef struct {
  TokenStream** expressions;
  size_t count;
  size_t capacity;
} ExprStream;

#define da_append(xs, x)\
  do { \
    if (xs.count >= xs.capacity) {\
      if (xs.capacity == 0) xs.capacity = 256;\
      else xs.capacity *= 2;\
      xs.expressions = realloc(xs.expressions, xs.capacity*sizeof(*xs.expressions));\
    }\
    xs.expressions[xs.count++] = x;\
  } while (0)

// Create a Symbol Table

/*
 * (\x . x y)       -- x is bound, y is free 
 *    | alpha convert
 *    v
 * (\z . z y)   == (\y . y x)
 */

void env_add(Env** env, const char* name, Expr* value);
Expr* env_lookup(Env* env, const char* name);
void interpret(ExprStream* stream);
Expr* eval(Expr* expr, Env* env);
Expr* substitute(Expr* body, const char* var, Expr* value);

#endif
