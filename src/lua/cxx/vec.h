#ifndef __YUM_CXX_VARIANT_VECTOR_H__
#define __YUM_CXX_VARIANT_VECTOR_H__

#include <stdint.h>

typedef struct YcxxYum_Vec YcxxYum_Vec;

#ifdef __cplusplus
#include <vector>
#include "variant.h"

extern "C"
{
#endif // __cplusplus

  YcxxYum_Vec *YcxxYum_newVec();
  void YcxxYum_deleteVec(YcxxYum_Vec *v);
  void YcxxYum_pushIntVec(int64_t a, YcxxYum_Vec *v);
  void YcxxYum_pushNumberVec(double a, YcxxYum_Vec *v);
  void YcxxYum_pushBooleanVec(int a, YcxxYum_Vec *v);
  void YcxxYum_pushStringVec(const char *a, YcxxYum_Vec *v);
  int64_t YcxxYum_sizeVec(YcxxYum_Vec *v);
  const char *YcxxYum_typeAtVec(YcxxYum_Vec *v, int64_t idx);
  int64_t YcxxYum_intAtVec(YcxxYum_Vec *v, int64_t idx);
  double YcxxYum_numberAtVec(YcxxYum_Vec *v, int64_t idx);
  int YcxxYum_booleanAtVec(YcxxYum_Vec *v, int64_t idx);
  const char *YcxxYum_stringAtVec(YcxxYum_Vec *v, int64_t idx);
  void YcxxYum_clearVec(YcxxYum_Vec *v);
  void YcxxYum_popBackVec(YcxxYum_Vec *v);

#ifdef __cplusplus
} /* extern "C" */


inline YcxxYum_Vec *cxxVecVariant2CVecVariant(const std::vector<YcxxVariantBase> &v) {
  YcxxYum_Vec* out = YcxxYum_newVec();
  for (auto &elem : v) {
    switch (elem.index()) {
      case 0: // int64_t
        YcxxYum_pushIntVec(std::get<int64_t>(elem), out);
        break;
      case 1: // double
        YcxxYum_pushNumberVec(std::get<double>(elem), out);
        break;
      case 2: // bool
        YcxxYum_pushBooleanVec(std::get<bool>(elem) ? 1 : 0, out);
        break;
      case 3: // std::string
        YcxxYum_pushStringVec(std::get<std::string>(elem).c_str(), out);
        break;
      default:
        // unsupported Variant type -> skip
        break;
    }
  }
  return out;
}

inline std::vector<Variant> CVecVariant2VecVariantCxx(YcxxYum_Vec* v) {
  std::vector<Variant> out;
  if (!v) return out;

  int64_t sz = YcxxYum_sizeVec(v);
  out.reserve(static_cast<size_t>(sz));

  for (int64_t i = 0; i < sz; ++i) {
    const char* t = YcxxYum_typeAtVec(v, i);
    if (!t) continue;

    if (strcmp(t, "int") == 0) {
      out.emplace_back(YcxxYum_intAtVec(v, i));
    } else if (strcmp(t, "number") == 0) {
      out.emplace_back(YcxxYum_numberAtVec(v, i));
    } else if (strcmp(t, "bool") == 0) {
      out.emplace_back(static_cast<bool>(YcxxYum_booleanAtVec(v, i)));
    } else if (strcmp(t, "string") == 0) {
      out.emplace_back(std::string(YcxxYum_stringAtVec(v, i)));
    }
  }
  return out;
}

#endif // __cplusplus

#endif // __YUM_CXX_VARIANT_VECTOR_H__