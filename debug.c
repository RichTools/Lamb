#include "debug.h"

#ifdef LOGGING
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
#else 
void log_reduction(ReductionType type, const char* label, Expr* expr)
{
}
#endif



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
        case EXPR_IMPORT:
            //printf("#import <");
            //printf("%s", expr->impt.filename);
            //printf(">");
            break;
    }
}

void print_indent(int count, char ch, const char* string, char* value)
{
    putchar('|');
    for (int i = 0; i < count; i++) putchar(ch);
    printf("%s: %s\n",string, value);
}

#ifdef LOGTREES
int print_expr_debug(const Expr* expr, int indent)
{
    if (!expr)
    {
        //printf("%*s(null)\n", indent, "");
        return 0;
    }

    switch (expr->type)
    {
        case EXPR_IMPORT:
            //print_indent(indent, '-', "IMPORT", (char*)expr->impt.filename);
            break;
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
    return 1;
}
#else 
int print_expr_debug(const Expr* expr, int indent)
{
  return 0;
}
#endif

void expression_as_string(const Expr* e)
{
    switch (e->type)
    {
      case EXPR_VAR:    printf("EXPR_VAR "); break; // <name>
      case EXPR_ABS:    printf("EXPR_ABS "); break; // <function>
      case EXPR_APP:    printf("EXPR_APP "); break; // <application>
      case EXPR_DEF:    printf("EXPR_DEF "); break;  // <assignment>
      case EXPR_IMPORT: printf("EXPR_IMPORT"); break; // <import>
    }
}

