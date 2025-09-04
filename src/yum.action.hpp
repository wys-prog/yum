#pragma once

#include <functional>
#include <vector>

namespace yum {
template <typename T>
class Action {
private:
  std::function<T(T)> step;

public:
  inline Action() = default;
  inline Action(std::function<T(T)> fn) : step(fn) {}

  inline T run(T value) const {
    if (step) return step(value);
    return value;
  }

  inline Action<T> then(std::function<T(T)> next) const {
    return Action<T>([this, next](T input) {
      T out = run(input);
      return next(out);
    });
  }

  /* get returns a COPY */
  inline std::function<T(T)> get() const { return step; }
};

template <typename T>
class ActionString {
private:
  std::vector<std::function<T(T)>> actions;

public:
  inline ActionString<T> &then(const std::function<T(T)> &next) {
    actions.push_back(next);
    return *this;
  }

  inline ActionString<T> &then(const Action<T> &next) {
    actions.push_back(next.get());
    return *this;
  }

  inline T run(const T &v) {
    T now = v;
    for (auto &func : actions) {
      now = func(now);
    }
    return now;
  }

  inline ActionString() {}
  inline ActionString(const Action<T> &act) { actions.push_back(act.get()); }
  inline ActionString(const std::function<T(T)> &fn) { actions.push_back(fn); }
};
}