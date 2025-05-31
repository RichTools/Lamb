#include <stdio.h>
#include <string.h>

#define RICHBUILD_IMPLEMENTATION
#include "build/richBuild.h"
#include "parser.h"
#include "lexer.h"

void parse_file(const char* filename)
{
  FILE *fptr;
  fptr = fopen(filename, "r");
  if (fptr == NULL)
  {
    fprintf(stderr, "Failed to Open File \n");
    exit(1);
  }

  char contents[100];
  while(fgets(contents, 100, fptr)) {
    // strip new lines
    size_t len = strlen(contents);
    if (len > 0 && contents[len - 1] == '\n')
    {
      contents[len-1] = '\0';
    }
    //parse(contents);
  }
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
  int pos = 0;
  shift(&argc, &argv);
  
  if (argc == 0) // interpreter mode 
  {
    printf("\\>: ");
    if (fgets(line, sizeof(line), stdin)) 
    {
      size_t len = strlen(line);
      if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
      const char* line = "(\\x . x\n)(d)";
      TokenStream tokens = tokenise(line);
      parse_expression(tokens, &pos);
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
