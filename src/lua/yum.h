#ifndef __YUM_H__
#define __YUM_H__

#include <stdint.h>

typedef uint64_t Yumuid;

typedef struct {
  int16_t code;
  Yumuid  uid;
} YumErr;

#ifdef __cplusplus
extern "C" {
#endif

YumErr        Yum_InitSubsystem();
YumErr        Yum_NewWorld();
YumErr        Yum_RunCode(Yumuid world, Yumuid src);
YumErr        Yum_Compile(const char *src);
YumErr        Yum_CompileString(const char *str);
YumErr        Yum_Run();
YumErr        Yum_LoadLibs(Yumuid libid);
void          Yum_DestroyWorld(Yumuid world);
const char   *Yum_GetError(YumErr e);
void          Yum_Shutdown();

#ifdef __cplusplus
} /* End of C++ */
#endif

#endif // __YUM_H__