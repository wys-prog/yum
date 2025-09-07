#include <string>
#include <vector>
#include <cstdint>
#include <variant>

#include "vec.h"
#include "variant.h"

struct YcxxYum_Vec {
  std::vector<YcxxVariantBase> v;
};

extern "C" {
  YcxxYum_Vec *YcxxYum_newVec() {
    return new YcxxYum_Vec();
  }

  void YcxxYum_deleteVec(YcxxYum_Vec *v) {
    if (v) delete v;
  }

  void YcxxYum_pushIntVec(int64_t a, YcxxYum_Vec *v) {
    if (v) v->v.push_back(a);
  }
  
  void YcxxYum_pushNumberVec(double a, YcxxYum_Vec *v) {
    if (v) v->v.push_back(a);
  }

  void YcxxYum_pushBooleanVec(int a, YcxxYum_Vec *v) {
    if (v) v->v.push_back((bool)a);
  }
  
  void YcxxYum_pushStringVec(const char *a, YcxxYum_Vec *v) {
    if (v) v->v.push_back(std::string(a));
  }

  void YcxxYum_pushVariantVec(Variant *a, YcxxYum_Vec *v) {
    std::string type = YcxxYum_typeOf(a);
    if (type == "int") YcxxYum_pushIntVec(YcxxYum_asInt(a), v);
    else if (type == "number") YcxxYum_pushNumberVec(YcxxYum_asNumber(a), v);
    else if (type == "bool") YcxxYum_pushBooleanVec(YcxxYum_asBool(a), v);
    else if (type == "string") YcxxYum_pushStringVec(YcxxYum_asString(a), v);
    else YcxxYum_pushIntVec(-1, v); /* Assuming -1 as default value. */
  }

  int64_t YcxxYum_sizeVec(YcxxYum_Vec *v) {
    if (v) return (int64_t)v->v.size();
    return -1;
  }
  
  const char* YcxxYum_typeAtVec(YcxxYum_Vec *v, int64_t idx) {
    if (!v || idx < 0 || idx >= (int64_t)v->v.size()) return "invalid";

    auto &val = v->v[idx];
    if (std::holds_alternative<int64_t>(val)) return "int";
    if (std::holds_alternative<double>(val)) return "number";
    if (std::holds_alternative<bool>(val)) return "bool";
    if (std::holds_alternative<std::string>(val)) return "string";
    return "unknown";
  }

  int64_t YcxxYum_intAtVec(YcxxYum_Vec *v, int64_t idx) {
    if (!v || idx < 0 || idx >= (int64_t)v->v.size()) return 0;
    auto &val = v->v[idx];
    if (auto p = std::get_if<int64_t>(&val)) return *p;
    return 0;
  }

  double YcxxYum_numberAtVec(YcxxYum_Vec *v, int64_t idx) {
    if (!v || idx < 0 || idx >= (int64_t)v->v.size()) return 0.0;
    auto &val = v->v[idx];
    if (auto p = std::get_if<double>(&val)) return *p;
    return 0.0;
  }

  int YcxxYum_booleanAtVec(YcxxYum_Vec *v, int64_t idx) {
    if (!v || idx < 0 || idx >= (int64_t)v->v.size()) return 0;
    auto &val = v->v[idx];
    if (auto p = std::get_if<bool>(&val)) return *p ? 1 : 0;
    return 0;
  }

  const char* YcxxYum_stringAtVec(YcxxYum_Vec *v, int64_t idx) {
    if (!v || idx < 0 || idx >= (int64_t)v->v.size()) return "";
    auto &val = v->v[idx];
    if (auto p = std::get_if<std::string>(&val)) return p->c_str();
    return "";
  }

  void YcxxYum_clearVec(YcxxYum_Vec *v) {
    if (v) v->v.clear();
  }

  void YcxxYum_popBackVec(YcxxYum_Vec *v) {
    if (v && !v->v.empty()) v->v.pop_back();
  }
}
