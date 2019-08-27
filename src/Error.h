#pragma once

// std::optional doesn't work for me!
enum struct MAYBE { PRESENT, ABSENT };
template <typename T>
struct Maybe {
  Maybe(T _data) : tag(MAYBE::PRESENT), val(_data) {}
  Maybe() : tag(MAYBE::ABSENT), val() {}
  MAYBE tag;
  T val;
};