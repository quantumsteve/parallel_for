#include <Poco/Mutex.h>

// Typedefs for ThreadPool 
typedef Poco::FastMutex Mutex;
typedef Poco::Mutex RecursiveMutex;


// If the compiler supports lambdas, use them.
#ifndef _USE_LAMBDAS
  #if (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 5) || _MSC_VER >= 1600
    #define _USE_LAMBDAS 1
  #elif __clang__
    #define _USE_LAMBDAS __has_feature(cxx_lambdas)
  #else
    #define _USE_LAMBDAS 0
  #endif
#endif

// The syntax used to define a pragma within a macro is different on windows and GCC
#ifdef _MSC_VER
  #define PRAGMA __pragma
#else //_MSC_VER
  #define PRAGMA(x) _Pragma(#x)
#endif //_MSC_VER

#ifdef _USE_LAMBDAS
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
#else
  #define PARALLEL_FRAMEWORK "none"
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

#else

#define PARALLEL_FRAMEWORK "deprecated behavior"

#define BEGIN_PARALLEL_FOR(condition,start,end,variable) \
PRAGMA(omp parallel for if (condition) ) \
for (int variable = start; variable < end; ++variable)

#define END_PARALLEL_FOR

#endif
