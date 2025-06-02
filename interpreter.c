#include "interpreter.h"
#include "diagnostics.h"
#include "debug.h"
#include <string.h>

void env_add(Env** env, const char* name, Expr* value)
{
  Env* entry = malloc(sizeof(Env));
  entry->name = name;
  entry->value = value;
  entry->next = *env;
  *env = entry;
}

Expr* env_lookup(Env* env, const char* name) {
    for (; env != NULL; env = env->next) {
        if (strcmp(env->name, name) == 0)
            return env->value;
    }
    return NULL;  // Unbound variable
}


// - Determine whether each Var node is Free, or Bound
// - Substitute parts via varaible look up
// - Alpha Convert if needed 
// - Beta Reduction


Expr* eval(Expr* expr, Env* env)
{
  switch (expr->type)
  {
    case EXPR_VAR: 
    {
        printf("Looking up %s", expr->var.name);
        Expr* val = env_lookup(env, expr->var.name);
        if (!val)
        {
          // free var
          return expr;
        }
        print_expr(val);
        return eval(val, env);
    }
    case EXPR_ABS: 
    {
      return expr;
    }
    case EXPR_APP: 
    {
        Expr* func = eval(expr->app.func, env);
        Expr* arg = eval(expr->app.arg, env);

        if (func->type != EXPR_ABS)
        {
          report_interp(DIAG_ERROR, "Attempted to apply a non-function");
        }

        // Beta Reduction
        Expr* body = substitute(func->abs.body, func->abs.param, arg);
        return eval(body, env);
    }
    default:
      report_interp(DIAG_ERROR, "Unknown Expression Type");
  }
}

Expr* substitute(Expr* body, const char* var, Expr* value)
{
  return body;
}


void interpret(ExprStream* stream)
{
  Env* env = NULL;

  for (int i = 0; i < stream->count; ++i)
  {
    int pos = i;
    Expr* expr = parse_expression(*stream->expressions[i], &pos);
    if (!expr) continue;


    if (expr->type == EXPR_DEF)
    {
      printf("Adding %s to the Environment\n", expr->def.name);
      env_add(&env, expr->def.name, expr->def.value);
    }
    else 
    {
      printf("Evaluating Expression\n");
      Expr* result = eval(expr, env);
      print_expr_debug(result, 0);
    }
    i = pos - 1;
  }
}



//Env* env = NULL;

//for (int i = 0; i < stream.count; i++) {
//    int pos = i;
//    Expr* expr = parse_expression(stream, &pos);
//
//    if (expr && expr->type == EXPR_DEF) {
//        env_add(&env, expr->def.name, expr->def.value);
//    } else {
//        eval(expr, env);
//    }
//
//    i = pos; // move to next expression
//}

