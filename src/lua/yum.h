#ifndef __YUM_H__
#define __YUM_H__

#include <stdint.h>

#define YUM_UID_NULL ((Yumuid)(0x00))
#define YUM_LIBS_YES ((uint8_t)(1))
#define YUM_LIBS_NOO ((uint8_t)(0))

typedef uint64_t Yumuid;

typedef struct {
  int16_t code;
  Yumuid  uid;
} YumErr;

#ifdef __cplusplus
extern "C" {
#endif

YumErr        Yum_InitSubsystem(const char *homedir);
YumErr        Yum_NewWorld();
YumErr        Yum_RunCode(Yumuid world, Yumuid src);
YumErr        Yum_Compile(const char *src, uint8_t libs);
YumErr        Yum_CompileString(const char *str, uint8_t libs);
YumErr        Yum_RunFast(Yumuid uid);
//YumErr        Yum_LoadLibs(Yumuid libid);
void          Yum_DestroyWorld(Yumuid world);
const char   *Yum_GetError(YumErr e);
void          Yum_Shutdown();

#ifdef __cplusplus
} /* End of C++ */
#endif

#endif // __YUM_H__