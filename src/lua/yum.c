#include <stdint.h>

#include "yumv.h"
#include "yumexport.h"
#include "cxx/yuxx.h"

YUM_API int32_t     Yum_initSubsystem       (void)                {   return YcxxYum_initSubsystem();       }
YUM_API void        Yum_shutdownSubsystem   (void)                {   return YcxxYum_shutdownSubsystem();   }
YUM_API void        Yum_pushInt             (int64_t a)           {   return YcxxYum_pushInt(a);            }
YUM_API void        Yum_pushNumber          (double a)            {   return YcxxYum_pushNumber(a);         }
YUM_API void        Yum_pushString          (const char *a)       {   return YcxxYum_pushString(a);         }
YUM_API void        Yum_pushBoolean         (int16_t a)           {   return YcxxYum_pushBoolean(a);        }
YUM_API int32_t     Yum_loadString          (const char *str)     {   return YcxxYum_loadString(str);       }
YUM_API int32_t     Yum_loadFile            (const char *src)     {   return YcxxYum_loadFile(src);         }
YUM_API int32_t     Yum_call                (const char *name)    {   return YcxxYum_call(name);            }