#ifndef __YUM_VERSION_H__
# define __YUM_VERSION_H__
# define YUM_VERSION_ENCODE(major, minor, patch) ((major) * 10000 + (minor) * 100 + (patch))
# ifndef YUM_VERSION_MAJOR
#  define YUM_VERSION_MAJOR 1
# endif // YUM_VERSION_MAJOR
# ifndef YUM_VERSION_MINOR
#  define YUM_VERSION_MINOR 2
# endif // YUM_VERSION_MINOR
# ifndef YUM_VERSION_PATCH
#  define YUM_VERSION_PATCH 0
# endif // YUM_VERSION_PATCH
# ifndef YUM_BRANCH
#  define YUM_BRANCH "main"
# endif // YUM_BRANCH
# ifndef YUM_STUDIO
#  define YUM_STUDIO "yum_official"
# endif // YUM_STUDIO
# ifndef YUM_VERSION_BUILD
#  define YUM_VERSION (YUM_VERSION_ENCODE(YUM_VERSION_MAJOR, YUM_VERSION_MINOR, YUM_VERSION_PATCH))
# else
#  define YUM_VERSION YUM_VERSION_BUILD
# endif
#endif // __YUM_VERSION_H__