#define RICHBUILD_IMPLEMENTATION
#include "richBuild.h"

// -DLOGGING flag for logging enable
// -DLOGTREES flag for logging trees
#define cflags "-DLOGGING -Wall"
#define executable_name "Lamb"

void BUILD_PROJECT() {
  char* files = READ_FILES("../");
  COMPILE("gcc", files, cflags, executable_name, NULL);
  CLEANUP();
}

int main() {
  BUILD_PROJECT();
  return 0;
}
