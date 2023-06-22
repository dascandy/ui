#pragma once

#include <math.h>
#include <memory>

template <typename T>
class Var : private std::enable_shared_from_this<Var<T>> {
public:
    virtual T operator*() const = 0;
    virtual ~Var() = default;
};

template <typename T>
using Val = std::shared_ptr<Var<T>>;

template <typename T>
class VarRef : public Var<T> {
private:
    const Val<T> &value;
public:
    VarRef(const Val<T> &value) : value(value) {}
    T operator*() const { return **value; }
};

template <typename T, typename U>
class VarConv : public Var<T> {
private:
	Val<U> value;
public:
	VarConv(Val<U> value) : value(value) {}
	T operator*() const { return **value; }
};

template <typename T>
class VarNum : public Var<T> {
private:
  T val;
public:
  VarNum(T val) : val(val) {}
  T operator*() const { return val; }
};

template <typename T, typename F, typename... Ts>
class VarFunc : public Var<T> {
public:
  F func;
  std::tuple<Ts...> ts;
  VarFunc(F func, Ts... ts) 
  : func(func)
  , ts(ts...) {
  }
  template <typename U>
  static auto unwrap(const Val<U>& type) {
    return **type;
  }
  template <typename U>
  static auto unwrap(const U& type) {
    return type;
  }
  T operator*() const { 
    return std::apply([this](auto... args) {
      return func(unwrap(args)...);
    }, ts);
  }
};

template <typename T>
class VarSwitch : public Var<T> {
public:
  const Val<int64_t>& time;
  int64_t switchTime;
  mutable Val<T> before;
  Val<T> after;
  VarSwitch(const Val<int64_t>& time, int64_t switchTime, Val<T> before, Val<T> after) 
  : time(time)
  , switchTime(switchTime)
  , before(before)
  , after(after)
  {
  }
  T operator*() const {
    if (**time < switchTime) {
      return **before;
    } else {
      before.reset();
      return **after;
    }
  }
};

template <typename T>
Val<T> varswitch(const Val<int64_t>& time, int64_t switchTime, Val<T> before, Val<T> after) {
  return std::make_shared<VarSwitch<T>>(time, switchTime, std::move(before), std::move(after));
}

template <typename T>
Val<T> varref(const Val<T> &val) {
  return std::make_shared<VarRef<T>>(val);
}

template <typename T>
Val<T> val(T val) {
  return std::make_shared<VarNum<T>>(val);
}

template <typename T, typename F, typename... Ts>
Val<T> varfunc(F func, Ts... args) {
  return std::make_shared<VarFunc<T, F, Ts...>>(func, args...);
}

template <typename T, typename U>
Val<T> convert(Val<U> value) {
	return std::make_shared<VarConv<T, U>>(value);
}


