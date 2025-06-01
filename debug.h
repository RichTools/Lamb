#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

#define todo(msg) \
    do { \
        fprintf(stderr, "TODO: %s at %s:%d\n", msg, __FILE__, __LINE__); \
        assert(0); \
    } while (0)


void print_expr(const Expr* expr);
void print_indent(int count, char ch, const char* string, char* value);
void print_expr_debug(const Expr* expr, int indent);

#endif // DEBUG_H
