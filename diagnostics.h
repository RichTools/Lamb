#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#define COLOR_RED    "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET  "\x1b[0m"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef enum {
    DIAG_ERROR,
    DIAG_WARNING,
    DIAG_NOTE
} DiagSeverity;

void report_diag(DiagSeverity severity, int pos, const char* msg);
void report_interp(DiagSeverity severity, const char* msg);

#endif // DIAGNOSTICS_H
