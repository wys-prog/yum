#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yum.h"
#include "yumv.h"

static void run_file(const char* path) {
  YumErr err = Yum_Compile(path, YUM_LIBS_YES);
  if (err.code != 0) {
    fprintf(stderr, "err: Yum: cannot compile file '%s'\nerr: code:\t%d\nerr: what:\t%s\n",
        path, err.code, Yum_GetError(err));
    exit(err.code);
  }

  YumErr world = Yum_NewWorld();
  if (world.code != 0) {
    fprintf(stderr, "err: Yum: cannot create world\nerr: code:\t%d\nerr: what:\t%s\n",
        world.code, Yum_GetError(world));
    exit(world.code);
  }

  YumErr ex = Yum_RunCode(world.uid, err.uid);
  if (ex.code != 0) {
    fprintf(stderr, "err: Yum: cannot run code\nerr: code:\t%d\nerr: what:\t%s\n",
        ex.code, Yum_GetError(ex));
    exit(ex.code);
  }
}

static void repl(void) {
  char buffer[1024];
  printf("Yum interactive shell (type 'exit' to quit)\n");
  while (1) {
    printf("yum> ");
    if (!fgets(buffer, sizeof(buffer), stdin)) break;
    if (strncmp(buffer, "exit", 4) == 0) break;

    YumErr err = Yum_CompileString(buffer, YUM_LIBS_YES);
    if (err.code != 0) {
      fprintf(stderr, "err: Yum: compile failed\nerr: code:\t%d\nerr: what:\t%s\n",
          err.code, Yum_GetError(err));
      continue;
    }

    YumErr world = Yum_NewWorld();
    if (world.code != 0) {
      fprintf(stderr, "err: Yum: world failed\nerr: code:\t%d\nerr: what:\t%s\n",
          world.code, Yum_GetError(world));
      continue;
    }

    YumErr ex = Yum_RunCode(world.uid, err.uid);
    if (ex.code != 0) {
      fprintf(stderr, "err: Yum: execution failed\nerr: code:\t%d\nerr: what:\t%s\n",
          ex.code, Yum_GetError(ex));
      continue;
    }
  }
}

int main(int argc, char* argv[]) {
  printf("Yum -- %d.%d.%d\n", YUM_VERSION_MAJOR, YUM_VERSION_MINOR, YUM_VERSION_PATCH);

  YumErr init = Yum_InitSubsystem("./");
  if (init.code != 0) {
    fprintf(stderr, "err: Yum: cannot initialize subsystem\nerr: code:\t%d\nerr: what:\t%s\n",
        init.code, Yum_GetError(init));
    return init.code;
  }

  if (argc > 1) {
    run_file(argv[1]);
  } else {
    repl();
  }

  Yum_Shutdown();
  return 0;
}
