#include <vector>
#include <mutex>
#include <cstdio>
#include <algorithm>
#include <type_traits>

template <typename T>
struct Observed;

template <typename T>
struct Observer {
  Observer(Observed<T>& observed) 
  : observed(observed)
  {
    observed.observers.push_back(this);
  }
  virtual ~Observer() {
    *std::find(observed.observers.begin(), observed.observers.end(), this) = observed.observers.back();
    observed.observers.pop_back();
  }
  virtual void OnActionStart(T&) {}
  virtual void OnActionEnd(T&) {}
  Observed<T>& observed;
};

template <typename T>
struct SimpleObserver : Observer<T> {
  SimpleObserver(Observed<T>& observed, std::function<void(T&)> onUpdate)
  : Observer<T>(observed)
  , onUpdate(onUpdate)
  {
    onUpdate(***observed);
  }
  void OnActionEnd(T& t) override { onUpdate(t); }
  std::function<void(T&)> onUpdate;
};

template <typename T>
struct Observed {
  struct Using {
    Using(Observed& obj)
    : obj(obj)
    , l(obj.mutex)
    {
      obj.AddUsing();
    }
    ~Using() {
      obj.RemoveUsing();
    }
    Observed& obj;
    T* operator*() { return &obj.member; }
    T* operator->() { return &obj.member; }
    std::unique_lock<std::mutex> l;
  };
  template <typename U>
  Observed(const U& newValue) requires (std::is_convertible_v<U, T>) {
    member = newValue;
  }
  template <typename U>
  Observed(U&& newValue) requires (std::is_convertible_v<U, T>) {
    member = std::move(newValue);
  }
  template <typename U>
  Observed<T>& operator=(const U& newValue) requires (std::is_convertible_v<U, T>) {
    auto handle = transaction();
    member = newValue;
    return *this;
  }
  template <typename U>
  Observed<T>& operator=(U&& newValue) requires (std::is_convertible_v<U, T>) {
    auto handle = transaction();
    member = std::move(newValue);
    return *this;
  }
  Using operator*() { return Using(*this); }
  Using operator->() { return Using(*this); }
  Using transaction() { return Using(*this); }
  T member;
  std::vector<Observer<T>*> observers;
  void AddUsing() {
    for (auto& o : observers)
      o->OnActionStart(member);
  }
  void RemoveUsing() {
    for (auto& o : observers)
      o->OnActionEnd(member);
  }
  std::mutex mutex;
};

