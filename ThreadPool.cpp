#include <iostream>
#include "tbb/task_group.h"

using namespace tbb;

long Fib(long n) {
  if (n < 2) {
    return n;
  } else {
    long x, y;
    task_group g;
    g.run([&] { x = Fib(n - 1); }); // spawn a task
    g.run([&] { y = Fib(n - 2); }); // spawn another task
    g.wait();                       // wait for both tasks to complete
    return x + y;
  }
}

int main() {
  std::cout << Fib(20) << std::endl;
  return 0;
}
