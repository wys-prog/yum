#ifndef __YUM_CXX_VARIANT_H__
#define __YUM_CXX_VARIANT_H__

typedef struct Variant Variant;
#include <stdint.h>

#ifdef __cplusplus
#include <string>
#include <cstring>
#include <variant>

typedef std::variant<int64_t, double, bool, std::string> YcxxVariantBase;

struct Variant {
  YcxxVariantBase var;
  Variant()                     : var(0) { }
  Variant(int64_t v)            : var(v) { }
  Variant(double v)             : var(v) { }
  Variant(bool v)               : var(v) { }
  Variant(const std::string& v) : var(v) { }
};

extern "C" {
#endif // __cplusplus

  Variant *YcxxYum_newVariant();
  void YcxxYum_deleteVariant(Variant *v);
  void YcxxYum_setInt(Variant *v, int64_t i);
  void YcxxYum_setNumber(Variant *v, double d);
  void YcxxYum_setBool(Variant *v, int b);
  void YcxxYum_setString(Variant *v, const char *s);
  const char *YcxxYum_typeOf(Variant *v);
  int64_t YcxxYum_asInt(Variant *v);
  double YcxxYum_asNumber(Variant *v);
  int YcxxYum_asBool(Variant *v);
  const char *YcxxYum_asString(Variant *v);

#ifdef __cplusplus
}

inline YcxxVariantBase CVariant2CxxVariant(Variant* v) {
  if (!v) return {}; // default empty variant
  const char* t = YcxxYum_typeOf(v);
  if (!t) return {};

  if (strcmp(t, "int") == 0) {
    return YcxxYum_asInt(v);
  } else if (strcmp(t, "number") == 0) {
    return YcxxYum_asNumber(v);
  } else if (strcmp(t, "bool") == 0) {
    return static_cast<bool>(YcxxYum_asBool(v));
  } else if (strcmp(t, "string") == 0) {
    return std::string(YcxxYum_asString(v));
  }

  return {}; // unknown type
}

inline Variant* cxxVariant2CVariant(const YcxxVariantBase& v) {
  Variant* out = YcxxYum_newVariant();
  if (!out) return nullptr;

  std::visit([&](auto&& value){
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, int64_t>) {
      YcxxYum_setInt(out, value);
    } else if constexpr (std::is_same_v<T, double>) {
      YcxxYum_setNumber(out, value);
    } else if constexpr (std::is_same_v<T, bool>) {
      YcxxYum_setBool(out, value ? 1 : 0);
    } else if constexpr (std::is_same_v<T, std::string>) {
      YcxxYum_setString(out, value.c_str());
    }
  }, v);

  return out;
}


#endif // __cplusplus

#endif // __YUM_CXX_VARIANT_H__
