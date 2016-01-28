#include <iostream>
#include "parallel.h"

class SafeWS {
public:
  bool threadSafe() const { return true; }
};

class UnsafeWS {
public:
  bool threadSafe() const { return false; }
};

int main() {

  auto ws4 = std::make_shared<SafeWS>();
  auto ws5 = std::make_shared<UnsafeWS>();

  bool test1 = ThreadSafe(ws4.get());
  bool test2 = ThreadSafe(ws5.get());
  bool test3 = ThreadSafe(ws4.get(), ws4.get());
  bool test4 = ThreadSafe(ws4.get(), ws5.get());
  bool test5 = ThreadSafe(ws4.get(), ws4.get(), ws4.get());
  bool test6 = ThreadSafe(ws4.get(), ws4.get(), ws5.get());

  std::cout << test1 << " " << test3 << " " << test5 << std::endl;
  std::cout << test2 << " " << test4 << " " << test6 << std::endl;

  return 0;
}
