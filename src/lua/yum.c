#include <stdint.h>

#include "yumv.h"
#include "yumexport.h"

#include "cxx/yuxx.h"
#include "cxx/vec.h"
#include "cxx/variant.h"

#pragma region Ycxx.LuaSubsystem

YUM_API int32_t Yum_initSubsystem(void) { return YcxxYum_initSubsystem(); }
YUM_API void Yum_shutdownSubsystem(void) { YcxxYum_shutdownSubsystem(); }
YUM_API void Yum_pushInt(int64_t a) { return YcxxYum_pushInt(a); }
YUM_API void Yum_pushNumber(double a) { return YcxxYum_pushNumber(a); }
YUM_API void Yum_pushString(const char *a) { return YcxxYum_pushString(a); }
YUM_API void Yum_pushBoolean(int a) { return YcxxYum_pushBoolean(a); }
YUM_API int32_t Yum_loadString(const char *str) { return YcxxYum_loadString(str); }
YUM_API int32_t Yum_loadFile(const char *src) { return YcxxYum_loadFile(src); }
YUM_API int32_t Yum_call(const char *name) { return YcxxYum_call(name); }
YUM_API int32_t Yum_registerCSCallback(const char *name, YumCSharpFunc func) { return YcxxYum_registerCSCallback(name, func); }

#pragma endregion

YUM_API Variant *Yum_newVariant() { return YcxxYum_newVariant(); }
YUM_API void Yum_deleteVariant(Variant *v) { return YcxxYum_deleteVariant(v); }
YUM_API void Yum_setInt(Variant *v, int64_t i) { return YcxxYum_setInt(v, i); }
YUM_API void Yum_setNumber(Variant *v, double d) { return YcxxYum_setNumber(v, d); }
YUM_API void Yum_setBool(Variant *v, int b) { return YcxxYum_setBool(v, b); }
YUM_API void Yum_setString(Variant *v, const char *s) { return YcxxYum_setString(v, s); }
YUM_API const char *Yum_typeOf(Variant *v) { return YcxxYum_typeOf(v); }
YUM_API int64_t Yum_asInt(Variant *v) { return YcxxYum_asInt(v); }
YUM_API double Yum_asNumber(Variant *v) { return YcxxYum_asNumber(v); }
YUM_API int Yum_asBool(Variant *v) { return YcxxYum_asBool(v); }
YUM_API const char *Yum_asString(Variant *v) { return YcxxYum_asString(v); }

#pragma region Ycxx.Vector<Variant> Export

YUM_API YcxxYum_Vec *Yum_newVec(void) { return YcxxYum_newVec(); }
YUM_API void Yum_deleteVec(YcxxYum_Vec *v) { return YcxxYum_deleteVec(v); }
YUM_API void Yum_pushIntVec(int64_t a, YcxxYum_Vec *v) { return YcxxYum_pushIntVec(a, v); }
YUM_API void Yum_pushNumberVec(double a, YcxxYum_Vec *v) { return YcxxYum_pushNumberVec(a, v); }
YUM_API void Yum_pushBooleanVec(int a, YcxxYum_Vec *v) { return YcxxYum_pushBooleanVec(a, v); }
YUM_API void Yum_pushStringVec(const char *a, YcxxYum_Vec *v) { return YcxxYum_pushStringVec(a, v); }
YUM_API int64_t Yum_sizeVec(YcxxYum_Vec *v) { return YcxxYum_sizeVec(v); }
YUM_API const char *Yum_typeAtVec(YcxxYum_Vec *v, int64_t idx) { return YcxxYum_typeAtVec(v, idx); }
YUM_API int64_t Yum_intAtVec(YcxxYum_Vec *v, int64_t idx) { return YcxxYum_intAtVec(v, idx); }
YUM_API double Yum_numberAtVec(YcxxYum_Vec *v, int64_t idx) { return YcxxYum_numberAtVec(v, idx); }
YUM_API int Yum_booleanAtVec(YcxxYum_Vec *v, int64_t idx) { return YcxxYum_booleanAtVec(v, idx); }
YUM_API const char *Yum_stringAtVec(YcxxYum_Vec *v, int64_t idx) { return YcxxYum_stringAtVec(v, idx); }
YUM_API void Yum_clearVec(YcxxYum_Vec *v) { return YcxxYum_clearVec(v); }
YUM_API void Yum_popBackVec(YcxxYum_Vec *v) { return YcxxYum_popBackVec(v); }

#pragma endregion