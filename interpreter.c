#include "interpreter.h"

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


void interpret(Expr* stream)
{
  
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

