#include "debug.h"

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
    }
}

void print_indent(int count, char ch, const char* string, char* value)
{
    putchar('|');
    for (int i = 0; i < count; i++) putchar(ch);
    printf("%s: %s\n",string, value);
}

void print_expr_debug(const Expr* expr, int indent)
{
    if (!expr)
    {
        printf("%*s(null)\n", indent, "");
        return;
    }

    switch (expr->type)
    {
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
}
