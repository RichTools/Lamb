#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
#include "debug.h"

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

void log_reduction(ReductionType type, const char* label, Expr* expr);

void env_add(Env** env, const char* name, Expr* value);
Expr* env_lookup(Env* env, const char* name);
void free_env(Env* env);

void interpret(ExprStream* stream);
Expr* eval(Expr* expr, Env* env);
void read_module(Expr* expr, Env* env);
Expr* eval_module(Expr* expr, Env** env);
Expr* copy_expr(Expr* expr);

// Set the current file being interpreted, used for resolving relative imports
char* resolve_relative_path(const char* current_file_path, const char* import_filename);
void set_current_file_path(const char* path);
bool file_is_readable(const char* path);
char* join_path(const char* a, const char* b);
bool ends_with(const char* s, const char* suffix);
char* with_ext(const char* filename, const char* new_ext);
static char* resolve_import_path(const char* import_filename);

Expr* beta_reduce(Expr* body, const char* var, Expr* value);
Expr* alpha_conversion(Expr* expr, const char* old_name, const char* new_name);
bool is_free_in(const char* name, Expr* expr);
Expr* eta_reduction(Expr* expr);

#endif
