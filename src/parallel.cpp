#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>


// The syntax used to define a pragma within a macro is different on windows and GCC
#ifdef _MSC_VER
#define PRAGMA __pragma
#else //_MSC_VER
#define PRAGMA(x) _Pragma(#x)
#endif //_MSC_VER

#ifdef OLD_BEHAVIOR

#define PARALLEL_FRAMEWORK "old behavior"

#define BEGIN_PARALLEL_FOR(condition,start,end,variable) \
PRAGMA(omp parallel for if (condition) ) \
for (int variable = start; variable < end; ++variable)
    
#define END_PARALLEL_FOR

#else

/*
   1. HAVE_TBB         - 3rdparty library, should be explicitly enabled
   2. HAVE_OPENMP      - integrated to compiler, defined iff HAVE_TBB is not defined and _OPENMP is defined
*/

#if defined HAVE_TBB
  #define PARALLEL_FRAMEWORK "tbb"
  #include "tbb/tbb.h"
#elif defined _OPENMP
  #define PARALLEL_FRAMEWORK "openmp"
  #define HAVE_OPENMP
  #include <omp.h>
#endif // PARALLEL_FRAMEWORK

/* ================================   parallel_for_  ================================ */

template <class T>
void parallel_for(bool parallel, int start, int end, const T& body)
{
  if( parallel )
  {
#ifdef HAVE_TBB
    tbb::parallel_for(start, end, body);
#else
    PRAGMA(omp parallel for)
    for (int i = start; i < end; ++i)
      body(i);
#endif
  }
  else
  {
    for (int i = start; i < end; ++i)
      body(i);
  }
}

#define BEGIN_PARALLEL_FOR(parallel,start,end,variable) \
parallel_for(parallel, start, end, [&](int variable)

#define END_PARALLEL_FOR \
);


#endif

void fillMatrix( std::vector<std::vector<double>>& matrix)
{
    std::size_t length = matrix.size();
    std::mt19937 generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    for (std::size_t i = 0; i < length; ++i)
    {
        for (std::size_t j = 0; j < length; ++j)
        {
            matrix[i][j] = distribution(generator);
        }
    }
}

int main()
{
    if (PARALLEL_FRAMEWORK)
        std::cout << "Parallel Framework: " << PARALLEL_FRAMEWORK << std::endl;
    
    std::size_t length(1000);
    std::vector<std::vector<double>> a(length, std::vector<double>(length)),
                                     b(length, std::vector<double>(length)),
                                     c(length, std::vector<double>(length));
    fillMatrix(a);
    fillMatrix(b);
    
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    BEGIN_PARALLEL_FOR(true,0,length,i)
    {
      for (std::size_t j = 0; j < length; ++j)
      {
        for (std::size_t k = 0; k < length; ++k)
        {
          c[i][j] += a[i][k] * b[k][j];
        }
      }
    }
    END_PARALLEL_FOR
    
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    
    
    std::cout << "serial fallback: " << std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    BEGIN_PARALLEL_FOR(false,0,length,i)
    {
        for (std::size_t j = 0; j < length; ++j)
        {
            for (std::size_t k = 0; k < length; ++k)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    END_PARALLEL_FOR
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    
    return 0;
}

