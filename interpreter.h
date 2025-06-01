#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

// Linked List of Entries to the Symbol Table
typedef struct EnvEntry {
    char* name;
    Expr* value;
    struct EnvEntry* next;
} Env;

typedef struct {
  Expr* expressions;
  int count;
} ExprStream;

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

#endif
