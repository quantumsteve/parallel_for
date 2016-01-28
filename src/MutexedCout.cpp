#include <iostream>
#include <mutex>
#include "parallel.h"

int main() {
  std::mutex aMutex;
  parallel_for(par, 0, 10, [&aMutex](std::size_t i) {
    std::lock_guard<std::mutex> lock(aMutex);
    std::cout << "Hello from task " << i << std::endl;
  });

  return 0;
}
