#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "interpreter.h"
#include "diagnostics.h"
#include "debug.h"


void log_reduction(ReductionType type, const char* label, Expr* expr)
{
    const char* prefix = "";
    switch(type) {
        case REDUCTION_BETA: prefix = "β > "; break;
        case REDUCTION_DELTA: prefix = "δ > "; break;
        case CONVERSION_ALPHA: prefix = "α > "; break;
        case REDUCTION_ETA: prefix = "η > "; break;
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
    // If it is undefined treat it as a constant
    return NULL;
}

void free_env(Env* env)
{
    while (env) {
        Env* next = env->next;
        free_expr(env->value);  
        free(env);              
        env = next;
    }
}


bool is_free_in(const char* name, Expr* expr)
{
  switch (expr->type)
  {
    case EXPR_ABS: 
    {
        if (strcmp(expr->abs.param, name) == 0)
        {
          return false; // it is bound 
        }
        else
        {
          return is_free_in(name, expr->abs.body);
        }
    }
    case EXPR_APP:
    {
        return is_free_in(name, expr->app.func) || is_free_in(name, expr->app.arg);
    }
    case EXPR_VAR:
    {
      return strcmp(expr->var.name, name) == 0;    
    }
    case EXPR_DEF:
      return false;
  }
  return false;
}



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
      return eval(eta_reduction(expr), env);
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
        body = eta_reduction(body);
        return eval(body, env);
    }
    default:
      report_interp(DIAG_ERROR, "Unknown Expression Type");
  }
  assert(0 && "Unreachable");
}

Expr* eta_reduction(Expr* expr)
{
    if (expr->type == EXPR_ABS)
    {
        Expr* body = expr->abs.body;

        if (body->type == EXPR_APP && body->app.arg->type == EXPR_VAR &&
            strcmp(body->app.arg->var.name, expr->abs.param) == 0 && 
            !is_free_in(expr->abs.param, body->app.func)) // x is not free in f
        {
            log_reduction(REDUCTION_ETA, "eta reduced", body->app.func);
            return body->app.func; // make a deep copy in case the pointer becomes invalid
        }
    }
    return expr;
}


Expr* alpha_conversion(Expr* expr, const char* old_name, const char* new_name)
{
  switch (expr->type)
  {
    case EXPR_VAR:
    {
        if (strcmp(expr->var.name, old_name) == 0)
        {
          Expr* new_var = malloc(sizeof(Expr));
          new_var->type = EXPR_VAR;
          new_var->var.name = strdup(new_name);
          return new_var;
        }
        return expr;
    }
    case EXPR_ABS:
    {
        Expr* new_abs = malloc(sizeof(Expr));
        new_abs->type = EXPR_ABS;
        if (strcmp(expr->abs.param, old_name) == 0)
        {
          new_abs->abs.param = strdup(new_name);
          new_abs->abs.body = alpha_conversion(expr->abs.body, old_name, new_name);
        }
        else
        {
          new_abs->abs.param = strdup(expr->abs.param);
          new_abs->abs.body = alpha_conversion(expr->abs.body, old_name, new_name);
        }
        return new_abs;
    }
    case EXPR_APP:
    {
        Expr* new_app = malloc(sizeof(Expr));
        new_app->type = EXPR_APP;
        new_app->app.func = alpha_conversion(expr->app.func, old_name, new_name);
        new_app->app.arg = alpha_conversion(expr->app.arg, old_name, new_name);
        return new_app;
    }
    default:
    {
        return expr;
    }
  }
}

// body[value/var]
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
        else if (is_free_in(body->abs.param, value)) 
        {
          // avoid variable capture
          char new_name[64];
          snprintf(new_name, sizeof(new_name), "%s_", body->abs.param); // add underscore for new name
          
          Expr* renamed_body = alpha_conversion(body->abs.body, body->abs.param, new_name);
          log_reduction(CONVERSION_ALPHA, "renamed variables", renamed_body);

          Expr* new_abs = malloc(sizeof(Expr));
          new_abs->type = EXPR_ABS;
          new_abs->abs.param = strdup(new_name);
          new_abs->abs.body = beta_reduce(renamed_body, var, value);
          return new_abs;
        }
        else
        {
          Expr* new_body = beta_reduce(body->abs.body, var, value);
          // create a new abs with the substituted body
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
    int pos = 0;

    Expr* expr = parse_expression(*stream->expressions[i], &pos);
    if (!expr) continue;
    printf("Input: ");
    print_expr(expr);
    printf("\n");

    if (expr->type == EXPR_DEF)
    {
      env_add(&env, expr->def.name, expr->def.value);
    }
    else 
    {
      Expr* result = eval(expr, env);
      printf("\nFinal Result: ");
      print_expr(result); printf("\n\n");
      free_expr(expr);
    }
  }
  free_env(env);
  env = NULL;
}

