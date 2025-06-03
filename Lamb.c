#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define RICHBUILD_IMPLEMENTATION
#include "build/richBuild.h"
#include "parser.h"
#include "lexer.h"
#include "interpreter.h"

void parse_file(const char* filename)
{
  FILE *fptr;
  fptr = fopen(filename, "r");
  if (fptr == NULL)
  {
    fprintf(stderr, "Failed to Open File \n");
    exit(1);
  }

  ExprStream exprs = {0};

  char contents[100];
  while(fgets(contents, 100, fptr)) {
    // strip new lines
    size_t len = strlen(contents);
    if (len > 0 && contents[len - 1] == '\n')
    {
      contents[len-1] = '\0';
      len--;
    }

    bool is_blank = true;
    for (size_t i = 0; i < len; ++i) {
        if (!isspace((unsigned char)contents[i])) {
            is_blank = false;
            break;
        }
    }
    if (is_blank) continue;

    TokenStream tokens = tokenise(contents);
    if (tokens.tokens == NULL)
    {
      fprintf(stderr, "Failed to tokenize input\n");
      continue;
    }
    
    // move tokens to the heap
    TokenStream* heap_tokens = malloc(sizeof(TokenStream));
    if (!heap_tokens)
    {
      fprintf(stderr, "Memory allocation failed\n");
      exit(1);
    }
    *heap_tokens = tokens;
    da_append(exprs, heap_tokens);
        
    //free_token_stream(&tokens);
  }
  interpret(&exprs); 
  fclose(fptr);
}

void shift(int* argc, char*** argv)
{
  if (*argc > 0) (*argc)--; (*argv)++;
}

int main(int argc, char** argv) 
{
  char line[256] = {0};
  const char* input_file = NULL;
  shift(&argc, &argv);
  
  if (argc == 0) // interpreter mode 
  {  
    while (1) 
    {
      printf("\\>: ");
      if (fgets(line, sizeof(line), stdin)) 
      {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        TokenStream tokens = tokenise(line);
        if (tokens.tokens == NULL) {
          fprintf(stderr, "Failed to tokenize input\n");
          return 1;
        }

        // heap allocate an expression
        ExprStream single_expr = {0};
        TokenStream* heap_tokens = malloc(sizeof(TokenStream));
        if (!heap_tokens) {
          fprintf(stderr, "Memory allocation failed\n");
          exit(1);
        }

        *heap_tokens = tokens;
        da_append(single_expr, heap_tokens);
        
        interpret(&single_expr);
            
        free_token_stream(&tokens);
        free(heap_tokens);
      }
    }
  }
  else 
  {
    while (argc > 0) 
    {
      if (strcmp(argv[0], "-i") == 0 && argc > 1)
      {
        input_file = argv[1];
        shift(&argc, &argv);
        shift(&argc, &argv);
        if (str_ends_with(input_file, ".l"))
        {
          parse_file(input_file);
        }
        else 
        {
          fprintf(stderr, "File should be a .l file\n");
        }
      }
      else 
      {
        if (strcmp(argv[0], "-i") == 0 && argc < 2)
        {
          fprintf(stderr, "Missing input file after -i\n");
        }
        else 
        {
          fprintf(stderr, "Unknown Argument: %s\n", argv[0]);
          shift(&argc, &argv);
        }
      }
    }
  }
  return 0;
}
