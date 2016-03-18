// Copyright 2016 Connor Taffe

#ifndef SRC_RESULT_H_
#define SRC_RESULT_H_

namespace basilisk {

// TODO(cptaffe): expand to include reasons
class Result {
 public:
  explicit Result(bool ok);
  bool isOk() const { return ok; }

 private:
  bool ok;
};

template <typename T>
class Optional {
 public:
  constexpr Optional();
  constexpr explicit Optional(T value);
  bool isOk() const { return ok; }
  T getValue() const { return value; }

 private:
  bool ok;
  T value;
};

template <typename T>
constexpr Optional<T>::Optional(T val) : ok{true}, value{val} {}

template <typename T>
constexpr Optional<T>::Optional() : ok{false} {}

}  // namespace basilisk

#endif  // SRC_RESULT_H_
