#include <variant>
#include <cstdint>
#include <string>

#include "variant.h"

extern "C" {
  Variant *YcxxYum_newVariant() {
    return new Variant();
  }

  void YcxxYum_deleteVariant(Variant *v) {
    delete v;
  }

  void YcxxYum_setInt(Variant *v, int64_t i) {
    if (v) v->var = i;
  }

  void YcxxYum_setNumber(Variant *v, double d) {
    if (v) v->var = d;
  }

  void YcxxYum_setBool(Variant *v, int b) {
    if (v) v->var = (b != 0);
  }

  void YcxxYum_setString(Variant *v, const char *s) {
    if (v) v->var = std::string(s ? s : "");
  }

  const char *YcxxYum_typeOf(Variant *v) {
    if (!v) return "null";
    if (std::holds_alternative<int64_t>(v->var)) return "int";
    if (std::holds_alternative<double>(v->var)) return "number";
    if (std::holds_alternative<bool>(v->var)) return "bool";
    if (std::holds_alternative<std::string>(v->var)) return "string";
    return "unknown";
  }

  int64_t YcxxYum_asInt(Variant *v) {
    if (!v) return 0;
    if (auto p = std::get_if<int64_t>(&v->var)) return *p;
    return 0;
  }

  double YcxxYum_asNumber(Variant *v) {
    if (!v) return 0.0;
    if (auto p = std::get_if<double>(&v->var)) return *p;
    return 0.0;
  }

  int YcxxYum_asBool(Variant *v) {
    if (!v) return 0;
    if (auto p = std::get_if<bool>(&v->var)) return *p ? 1 : 0;
    return 0;
  }

  const char *YcxxYum_asString(Variant *v) {
    if (!v) return "";
    if (auto p = std::get_if<std::string>(&v->var)) return p->c_str();
    return "";
  }
} // extern "C"
