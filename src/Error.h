#pragma once

enum struct MAYBE { PRESENT, ABSENT };

enum struct EITHER { SUCCESS, ERROR };

template <typename T>
struct Maybe {
  Maybe(T _data) : tag(MAYBE::PRESENT), val(_data) {}
  Maybe() : tag(MAYBE::ABSENT), val() {}
  ~Maybe() = default;
  MAYBE tag;
  T val;
};

template <typename T, typename T2>
struct Either {
  Either(T _data) : tag(EITHER::SUCCESS), val(_data) {}
  Either(T2 _data) : tag(EITHER::ERROR), val(_data) {}
  ~Either() = default;
  EITHER tag;
  union {
    T success;
    T2 error;
  } val;
};