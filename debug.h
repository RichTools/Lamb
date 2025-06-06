#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"

typedef enum {
    REDUCTION_NONE,
    REDUCTION_BETA,
    REDUCTION_DELTA,
    CONVERSION_ALPHA,
    REDUCTION_ETA,
} ReductionType;

#define todo(msg) \
    do { \
        fprintf(stderr, "TODO: %s at %s:%d\n", msg, __FILE__, __LINE__); \
        assert(0); \
    } while (0)


void print_expr(const Expr* expr);
void print_indent(int count, char ch, const char* string, char* value);
int print_expr_debug(const Expr* expr, int indent);
void expression_as_string(const Expr*);

#ifdef LOGTREES
#define LOG_TREE(expr) if (print_expr_debug(expr, 0)) printf("\n\n");
#else 
#define LOG_TREE printf("");
#endif

#endif // DEBUG_H
