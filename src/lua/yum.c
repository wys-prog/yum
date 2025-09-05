#include <stdio.h>

#include "yum.h"
#include "yumv.h"

int main(int argc, char *const argv[]) {
  printf("Yum -- %d.%d.%d", YUM_VERSION_MAJOR, YUM_VERSION_MINOR, YUM_VERSION_PATCH);

  YumErr err = Yum_InitSubsystem();
  
  if (err.code != 0) {
    fprintf(stderr, "err: cannot initialize Yum subsystem\nerr: code: \t%d\nerr: what:\t%s", err.code, Yum_GetError(err));
    return err.code;
  }
  
  Yum_Shutdown();

  return 0;
}