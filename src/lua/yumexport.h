#ifdef _
# ifndef __YUM_EXPORT_H__
#  define __YUM_EXPORT_H__
#  if defined(_WIN32) || defined(_WIN64)
#    define YUM_PLATFORM_WINDOWS 1
#  else
#    define YUM_PLATFORM_WINDOWS 0
#  endif
#  if defined(_MSC_VER)
#    define YUM_COMPILER_MSVC 1
#  else
#    define YUM_COMPILER_MSVC 0
#  endif
#  if defined(__GNUC__)
#    define YUM_COMPILER_GCC 1
#  else
#    define YUM_COMPILER_GCC 0
#  endif
#  if defined(__clang__)
#    define YUM_COMPILER_CLANG 1
#  else
#    define YUM_COMPILER_CLANG 0
#  endif
#  if YUM_PLATFORM_WINDOWS
#    if YUM_COMPILER_MSVC
#      define YUM_EXPORT __declspec(dllexport)
#      define YUM_IMPORT __declspec(dllimport)
#    else
#      define YUM_EXPORT __declspec(dllexport)
#      define YUM_IMPORT __declspec(dllimport)
#    endif
#  else
#    if YUM_COMPILER_GCC || YUM_COMPILER_CLANG
#      define YUM_EXPORT __attribute__((visibility("default")))
#      define YUM_IMPORT
#    else
#      define YUM_EXPORT
#      define YUM_IMPORT
#    endif
#  endif
#  ifdef YUM_BUILD_DLL
#    define YUM_API YUM_EXPORT
#  else
#    define YUM_API YUM_IMPORT
#  endif
# endif // __YUM_EXPORT_H__
#endif

#ifndef YUMEXPORT_H
#define YUMEXPORT_H

#if defined(_WIN32)
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

#endif
