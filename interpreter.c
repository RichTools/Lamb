#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <libgen.h>    
#include <unistd.h>   
#include <errno.h>

#include "interpreter.h"
#include "diagnostics.h"
#include "debug.h"

static Env* global_env = NULL;

void log_reduction(ReductionType type, const char* label, Expr* expr)
{
    const char* prefix = "";
    switch(type) {
        case REDUCTION_BETA: prefix = "β> "; break;
        case REDUCTION_DELTA: prefix = "δ> "; break;
        case CONVERSION_ALPHA: prefix = "α> "; break;
        case REDUCTION_ETA: prefix = "η> "; break;
        default: prefix = ">"; break;
    }
    printf("%s%s ", prefix, label);
    print_expr(expr);
    printf("\n");
}

void env_add(Env** env, const char* name, Expr* value)
{
    Env* entry = malloc(sizeof(Expr));
    if (!entry)
    {
        report_interp(DIAG_ERROR, "Env Memory allocation failed");
        return;
    }
    entry->name = strdup(name);
    if (!entry->name)
    {
        free(entry);
        report_interp(DIAG_ERROR, "Memory allocation failed");
        return;
    }
    entry->value = copy_expr(value);
    if (!entry->value)
    {
        free((void*)entry->name);
        free(entry);
        report_interp(DIAG_ERROR, "Memory allocation failed");
        return;
    }
    entry->next = *env;
    *env = entry;
}

Expr* env_lookup(Env* env, const char* name)
{
    for (; env != NULL; env = env->next) {
        if (strcmp(env->name, name) == 0) {
            return env->value;
        }
    }
    return NULL;
}

void free_env(Env* env)
{
    while (env) {
        Env* next = env->next;
        free_expr(env->value);
        free((void*)env->name);
        free(env);              
        env = next;
    }
}

Expr* copy_expr(Expr* expr)
{
    if (!expr) return NULL;

    Expr* new_expr = malloc(sizeof(Expr));
    new_expr->type = expr->type;

    switch (expr->type) {
        case EXPR_VAR:
            new_expr->var.name = strdup(expr->var.name);
            break;
        case EXPR_ABS:
            new_expr->abs.param = strdup(expr->abs.param);
            new_expr->abs.body = copy_expr(expr->abs.body);
            break;
        case EXPR_APP:
            new_expr->app.func = copy_expr(expr->app.func);
            new_expr->app.arg = copy_expr(expr->app.arg);
            break;
        case EXPR_DEF:
            new_expr->def.name = strdup(expr->def.name);
            new_expr->def.value = copy_expr(expr->def.value);
            break;
        case EXPR_IMPORT:
            new_expr->impt.filename = strdup(expr->impt.filename);
            break;
    }

    return new_expr;
}

char* resolve_relative_path(const char* current_file_path, const char* import_filename)
{
    char* path_copy = strdup(current_file_path);
    char* dir = dirname(path_copy);

    size_t full_len = strlen(dir) + strlen(import_filename) + 2;
    char* full_path = malloc(full_len);
    snprintf(full_path, full_len, "%s/%s", dir, import_filename);

    char* resolved = realpath(full_path, NULL);

    free(full_path);
    free(path_copy);
    return resolved;  // Remember to free this later!
}

Expr* eval_module(Expr* expr, Env** env)
{
    const char* filename = expr->impt.filename;

    FILE *fptr = fopen(filename, "r");
    if (!fptr) {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Import Failed: Could not read module '%s' (%s)", filename, strerror(errno));
        report_interp(DIAG_ERROR, err_msg);
        return NULL;
    }

    ExprStream module_exprs = {0};
  
    char contents[100];
    while (fgets(contents, sizeof(contents), fptr))
    {
        // strip new lines
        size_t len = strlen(contents);
        if (len > 0 && contents[len - 1] == '\n')
            contents[--len] = '\0';

        // Skip blank lines and comment lines starting with "--"
        size_t i = 0;
        while (i < len && isspace((unsigned char)contents[i])) i++;

        if (i == len || (contents[i] == '-' && contents[i + 1] == '-')) {
            continue; // Skip blank or comment line
        }

        TokenStream tokens = tokenise(contents);
        if (tokens.tokens == NULL)
        {
            report_interp(DIAG_ERROR, "Failed to tokenize input module\n");
        }

        // Allocate token stream on heap
        TokenStream* heap_tokens = malloc(sizeof(TokenStream));
        if (!heap_tokens) {
            fprintf(stderr, "Memory allocation failed\n");
            continue;
        }
        *heap_tokens = tokens;

        da_append(module_exprs, heap_tokens);
    }

    fclose(fptr);
  
    for (int i = 0; i < module_exprs.count; i++)
    {
        int pos = 0;
        Expr* parsed = parse_expression(*module_exprs.expressions[i], &pos);
        if (!parsed) break; // EoF 

        if (parsed->type == EXPR_DEF)
        {
            env_add(&global_env, parsed->def.name, copy_expr(parsed->def.value));
        }
        else 
        {
            report_interp(DIAG_ERROR, "Only definitions are allowed in module files");
        }

        free_expr(parsed);
    }

    // Clean up token streams
    for (int i = 0; i < module_exprs.count; i++) {
        free_token_stream(module_exprs.expressions[i]);
        free(module_exprs.expressions[i]);
    }
    free(module_exprs.expressions);
  
    return NULL;
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
        case EXPR_IMPORT:
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
            Expr* val = env_lookup(env, expr->var.name);
            if (!val)
            {
                log_reduction(REDUCTION_DELTA, "expanding", expr);
                return expr; // free var
            }
            log_reduction(REDUCTION_DELTA, "expanding", val);
            return val;
        }
        case EXPR_ABS: 
        {
            // recursive eval for nested exprs
            Expr* reduced_body = eval(expr->abs.body, env);
            Expr* new_abs = malloc(sizeof(Expr));
            new_abs->type = EXPR_ABS;
            new_abs->abs.param = strdup(expr->abs.param);
            new_abs->abs.body = reduced_body;
            return new_abs; // Defer eta reduction to avoid premature simplification
        }
        case EXPR_APP: 
        {
            log_reduction(REDUCTION_NONE, "applying", expr);
            Expr* func = eval(expr->app.func, env);
            
            if (func->type != EXPR_ABS)
            {
                Expr* arg = eval(expr->app.arg, env);
                Expr* new_app = malloc(sizeof(Expr));
                new_app->type = EXPR_APP;
                new_app->app.func = copy_expr(func);
                new_app->app.arg = copy_expr(arg);
                return new_app; // Return application without further evaluation
            }

            Expr* arg = eval(expr->app.arg, env);
            Expr* body = beta_reduce(func->abs.body, func->abs.param, arg);
            log_reduction(REDUCTION_BETA, "reduced", body);
            body = eta_reduction(body);
            return eval(body, env); // Continue evaluation after beta reduction
        }
        case EXPR_IMPORT:
        {
            return eval_module(expr, &global_env);
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
            !is_free_in(expr->abs.param, body->app.func) &&
            body->app.func->type == EXPR_ABS) // Only reduce if func is an abstraction
        {
            log_reduction(REDUCTION_ETA, "eta reduced", body->app.func);
            return copy_expr(body->app.func);
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

Expr* beta_reduce(Expr* body, const char* var, Expr* value)
{
    switch (body->type)
    {
        case EXPR_VAR: 
        {
            if (strcmp(body->var.name, var) == 0)
            {
                return copy_expr(value);
            }
            else 
            {
                Expr* copy = malloc(sizeof(Expr));
                if (!copy)
                {
                    report_interp(DIAG_ERROR, "Memory allocation failed");
                    return NULL;
                }
                copy->type = EXPR_VAR;
                copy->var.name = strdup(body->var.name);
                return copy;
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
                char new_name[64];
                snprintf(new_name, sizeof(new_name), "%s_", body->abs.param);
                Expr* renamed_body = alpha_conversion(body->abs.body, body->abs.param, new_name);
                log_reduction(CONVERSION_ALPHA, "renamed variables", renamed_body);
                Expr* new_abs = malloc(sizeof(Expr));
                new_abs->type = EXPR_ABS;
                new_abs->abs.param = strdup(new_name);
                new_abs->abs.body = beta_reduce(renamed_body, var, value);
                free_expr(renamed_body);
                return new_abs;
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
            Expr* new_arg = beta_reduce(body->app.arg, var, value);
            Expr* new_app = malloc(sizeof(Expr));
            new_app->type = EXPR_APP;
            new_app->app.func = new_func;
            new_app->app.arg = new_arg;
            return new_app;
        }
        default:
        {
            return body;
        }
    }
}

void interpret(ExprStream* stream)
{
    if (global_env == NULL) {
        global_env = NULL;
    }

    for (int i = 0; i < stream->count; ++i)
    {
        int pos = 0;
        Expr* expr = parse_expression(*stream->expressions[i], &pos);
        if (!expr) continue;

        if (expr->type == EXPR_DEF)
        {
            env_add(&global_env, expr->def.name, expr->def.value);
        }
        else 
        {
            Expr* result = eval(expr, global_env);
            print_expr(result); printf("\n\n");
            free_expr(expr);
            free_expr(result); // Free the result to prevent memory leaks
        }
    }
}

