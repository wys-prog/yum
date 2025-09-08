#ifndef ___YUM_CXXX_H___
#define ___YUM_CXXX_H___

#include "vec.h"
#include "variant.h"
#include "../yumexport.h"

typedef Variant *(*YumCSharpFunc)(const YcxxYum_Vec*);

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  YUM_API int32_t YcxxYum_initSubsystem(void);
  YUM_API void YcxxYum_shutdownSubsystem(void);
  YUM_API void YcxxYum_pushInt(int64_t a);
  YUM_API void YcxxYum_pushNumber(double a);
  YUM_API void YcxxYum_pushString(const char *a);
  YUM_API void YcxxYum_pushBoolean(int a);
  YUM_API int32_t YcxxYum_loadString(const char *str);
  YUM_API int32_t YcxxYum_loadFile(const char *src);
  YUM_API int32_t YcxxYum_call(const char *name);
  YUM_API int32_t YcxxYum_registerCSCallback(const char *name, YumCSharpFunc fn);

#ifdef __cplusplus
} /* extern "C" */
#endif // __cplusplus

#endif // ___YUM_CXXX_H___