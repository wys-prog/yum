#ifndef YUMEXPORT_H
#define YUMEXPORT_H

#if defined(_WIN32) || defined(_WIN64)
  #define YUM_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
  #define YUM_EXPORT __attribute__((visibility("default")))
#else
  #define YUM_EXPORT
#endif

#ifdef __cplusplus
  #define YUM_API extern "C" YUM_EXPORT
#else
  #define YUM_API YUM_EXPORT
#endif

#endif // YUMEXPORT_H
