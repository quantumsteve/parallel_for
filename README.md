parallel_for
============

Here is an attempt to create a generic parallel_for macro for Mantid. The goal is to minimize changes to the current openmp macros while falling back to threading building blocks on platforms without openmp. Once all of our compilers support lambda expressions, it gives up the option to replace the macros with functions.

add -DOLD_BEHAVIOR to emulate the current parallel macros.

To compile with gcc and openmp:
g++ -O3 -std=c++11 parallel.cpp -fopenmp

To compile with gcc and tbb:
g++ -O3 -std=c++11 parallel.cpp -ltbb -DHAVE_TBB

To compile with clang and tbb:
clang++ -O3 -std=c++11 parallel.cpp -ltbb -DHAVE_TBB
