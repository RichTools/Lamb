#include "interpreter.h"
#include "diagnostics.h"
#include "debug.h"
#include <string.h>
#include <assert.h>

typedef enum {
    REDUCTION_NONE,
    REDUCTION_BETA,
    REDUCTION_DELTA,
    REDUCTION_ALPHA
} ReductionType;

void log_reduction(ReductionType type, const char* label, Expr* expr) {
    const char* prefix = "";
    switch(type) {
        case REDUCTION_BETA: prefix = "β > "; break;
        case REDUCTION_DELTA: prefix = "δ > "; break;
        case REDUCTION_ALPHA: prefix = "α > "; break;
        default: prefix = "> "; break;
    }
    printf("%s%s ", prefix, label);
    print_expr(expr);
    printf("\n");
}

void env_add(Env** env, const char* name, Expr* value)
{
  Env* entry = malloc(sizeof(Env));
  entry->name = name;
  entry->value = value;
  entry->next = *env;
  *env = entry;
}

Expr* env_lookup(Env* env, const char* name)
{
    for (; env != NULL; env = env->next) {
        if (strcmp(env->name, name) == 0)
            return env->value;
    }
    return NULL;
}

// - Determine whether each Var node is Free, or Bound
// - Substitute parts via varaible look up
// - Alpha Convert if needed 
// - Beta Reduction
// - eta reduction

// Bound vs. free variables
// Delta (δ) rules 
// Beta (β) reduction
// Alpha (α) conversion
// Eta (η) conversion


Expr* eval(Expr* expr, Env* env)
{
  switch (expr->type)
  {
    case EXPR_VAR: 
    {
        //assert(env != NULL && "Failed to find environment");
        Expr* val = env_lookup(env, expr->var.name);
        if (!val)
        {
          log_reduction(REDUCTION_DELTA, "expanding", expr);
          // free var
          return expr;
        }
        log_reduction(REDUCTION_DELTA, "expanding", val);
        return eval(val, env);
    }
    case EXPR_ABS: 
    {
      return expr;
    }
    case EXPR_APP: 
    {
        log_reduction(REDUCTION_NONE, "applying", expr);

        Expr* func = eval(expr->app.func, env);
        Expr* arg = eval(expr->app.arg, env);
        
        if (func->type != EXPR_ABS)
        {
          expression_as_string(func);
          report_interp(DIAG_ERROR, "Attempted to apply a non-function");
        }

        // Beta Reduction
        Expr* body = beta_reduce(func->abs.body, func->abs.param, arg);
        log_reduction(REDUCTION_BETA, "reduced", body);
        return eval(body, env);
    }
    default:
      report_interp(DIAG_ERROR, "Unknown Expression Type");
  }
}

Expr* beta_reduce(Expr* body, const char* var, Expr* value)
{
  switch (body->type)
  {
    case EXPR_VAR: 
    {
      if (strcmp(body->var.name, var) ==  0)
      {
        return value; // substitute
      }
      else 
      {
        return body;
      }
    }
    case EXPR_ABS: 
    {
        if (strcmp(body->abs.param, var) == 0)
        {
          return body;  // shadowed, skip substitution
        }
        else
        {
          Expr* new_body = beta_reduce(body->abs.body, var, value);
          Expr* new_abs = malloc(sizeof(Expr));
          new_abs->type = EXPR_ABS;
          new_abs->abs.param = strdup(body->abs.param);
          new_abs->abs.body = new_body;
          return new_abs;
        }
    }
    case EXPR_APP:
    {
        Expr* new_func = beta_reduce(body->app.func, var, value);
        Expr* new_arg  = beta_reduce(body->app.arg, var, value);
        Expr* new_app  = malloc(sizeof(Expr));
        new_app->type = EXPR_APP;
        new_app->app.func = new_func;
        new_app->app.arg  = new_arg;
        return new_app;
    }
    default:
    {
      return body;
    }
  }
}


static Env* env = NULL;

void interpret(ExprStream* stream)
{
  for (int i = 0; i < stream->count; ++i)
  {
    int pos = i;
    Expr* expr = parse_expression(*stream->expressions[i], &pos);
    if (!expr) continue;
    printf("Input: ");
    print_expr(expr);
    printf("\n");

    if (expr->type == EXPR_DEF)
    {
      printf("Adding %s to the Environment\n", expr->def.name);
      env_add(&env, expr->def.name, expr->def.value);
    }
    else 
    {
      Expr* result = eval(expr, env);
      printf("\nFinal Result: ");
      print_expr(result); printf("\n\n");
    }
    i = pos - 1;
  }
}
