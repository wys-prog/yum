#ifndef __YUM_VERSION_H__
# define __YUM_VERSION_H__
# define YUM_VERSION_ENCODE(major, minor, patch) ((major) * 10000 + (minor) * 100 + (patch))
# ifndef YUM_VERSION
#   define YUM_VERSION YUM_VERSION_ENCODE(1, 2, 42)
# endif
# define YUM_VERSION_MAJOR (YUM_VERSION / 10000)
# define YUM_VERSION_MINOR ((YUM_VERSION / 100) % 100)
# define YUM_VERSION_PATCH (YUM_VERSION % 100)
# define YUM_BRANCH "main"
#   define YUM_STUDIO "yum_official"
#endif // __YUM_VERSION_H__