#include "diagnostics.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void report_diag(DiagSeverity severity, int pos, const char* msg)
{
    switch (severity) {
      case DIAG_ERROR:
        fprintf(stderr, COLOR_RED "[ERROR] %s\n" COLOR_RESET, msg);
        break;
      case DIAG_WARNING:
        fprintf(stderr, COLOR_YELLOW "[WARNING] %s\n" COLOR_RESET, msg);
        break;
      case DIAG_NOTE:
        fprintf(stderr, "[NOTE] %s\n", msg);
        break;
    }

  
    if (severity == DIAG_ERROR) exit(1);  // Stop on error
}

void report_interp(DiagSeverity severity, const char* msg)
{
    switch (severity) {
      case DIAG_ERROR:
        fprintf(stderr, COLOR_RED "[ERROR] Interpreter Warning: %s\n" COLOR_RESET, msg);
        break;
      case DIAG_WARNING:
        fprintf(stderr, COLOR_YELLOW "[WARNING] Interpreter Error: %s\n" COLOR_RESET, msg);
        break;
      case DIAG_NOTE:
        fprintf(stderr, "[NOTE] %s\n", msg);
        break;
    }

  
    if (severity == DIAG_ERROR) exit(1);  // Stop on error
}
