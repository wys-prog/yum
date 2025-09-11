/* vector.cpp — Definition */

#include <vector>
#include <functional>

#include "inc/variant.hpp"
#include "inc/vector.hpp"
#include "inc/yumdec.h"

namespace Yumcxx {
  Vector::Vector() : internal() {}
  Vector::~Vector() {}
  void Vector::append(const Variant &v) { internal.push_back(v); }
  void Vector::pop() { internal.pop_back(); }
  void Vector::clear() { internal.clear(); }
  int64_t Vector::size() const { return (int64_t)internal.size(); }
  Variant &Vector::at(int64_t i) { return internal.at(i); }
  const Variant &Vector::at(int64_t i) const { return internal.at(i); }
  void Vector::foreach(const std::function<void(const Variant&)> &f) const { for (const auto &v:internal) f(v); }
  void Vector::foreach(const std::function<void(Variant&)> &f) { for (auto &v:internal) f(v); }
}

extern "C" {

YUM_OUTATR YumVector *YumVector_new(void) {
  return new YumVector();
}

YUM_OUTATR void YumVector_delete(YumVector *vec) {
  delete vec;
}

YUM_OUTATR void YumVector_append(YumVector *vec, const YumVariant *value) {
  if (vec && value) vec->append(*value);
}

YUM_OUTATR void YumVector_pop(YumVector *vec) {
  if (vec) vec->pop();
}

YUM_OUTATR void YumVector_clear(YumVector *vec) {
  if (vec) vec->clear();
}

YUM_OUTATR int64_t YumVector_size(const YumVector *vec) {
  return vec ? vec->size() : 0;
}

YUM_OUTATR YumVariant *YumVector_at(YumVector *vec, int64_t index) {
  if (!vec) return nullptr;
  try {
    return &vec->at(index);
  } catch (...) {
    return nullptr;
  }
}

YUM_OUTATR const YumVariant *YumVector_at_const(const YumVector *vec, int64_t index) {
  if (!vec) return nullptr;
  try {
    return &vec->at(index);
  } catch (...) {
    return nullptr;
  }
}
}