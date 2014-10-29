#include <Poco/Mutex.h>

// Typedefs for ThreadPool 
typedef Poco::FastMutex Mutex;
typedef Poco::Mutex RecursiveMutex;


// If the compiler supports lambdas, use them.
#ifndef _USE_LAMBDAS
  #if __clang__
    #define _USE_LAMBDAS __has_feature(cxx_lambdas)
  #elif (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 5) || _MSC_VER >= 1600
    #define _USE_LAMBDAS 1
  #else
    #define _USE_LAMBDAS 0
  #endif
#endif

#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define THREADSAFE(...) GET_MACRO(__VA_ARGS__,TS3,TS2,TS1)(__VA_ARGS__)

#define TS1(ws1) \
  !ws1 || ws1->threadSafe()

#define TS2(ws1, ws2) \
  ( !ws1 || ws1->threadSafe() ) && \
  ( !ws2 || ws2->threadSafe() )

#define TS3(ws1, ws2, ws3) \
  ( !ws1 || ws1->threadSafe() ) && \
  ( !ws2 || ws2->threadSafe() ) && \
  ( !ws3 || ws3->threadSafe() )

// The syntax used to define a pragma within a macro is different on windows and GCC
#ifdef _MSC_VER
  #define PRAGMA __pragma
#else //_MSC_VER
  #define PRAGMA(x) _Pragma(#x)
#endif //_MSC_VER

#if _USE_LAMBDAS && !defined(OLD_BEHAVIOR)
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

template <class func>
void parallel_for(bool parallel, std::size_t start, std::size_t end, const func& body)
{
  if( parallel )
  {
#ifdef HAVE_TBB
    tbb::parallel_for(start, end, body);
#elif (defined(HAVE_OPENMP) && defined(_MSC_VER))
    PRAGMA(omp parallel for)
    for (long i = start; i < end; ++i)
      body(static_cast<std::size_t>(i));
#else
    PRAGMA(omp parallel for)
    for (std::size_t i = start; i < end; ++i)
      body(i);
#endif
  }
  else
  {
    for (std::size_t i = start; i < end; ++i)
      body(i);
  }
}

#define BEGIN_PARALLEL_FOR(parallel,start,end,variable) \
parallel_for(parallel, start, end, [&](std::size_t variable)

#define END_PARALLEL_FOR \
);

#else

#define PARALLEL_FRAMEWORK "deprecated behavior"

#ifdef _MSC_VER
  #define BEGIN_PARALLEL_FOR(condition,start,end,variable) \
  PRAGMA(omp parallel for if (condition) ) \
  for (long donotuseme = static_cast<long>(start); donotuseme < static_cast<long>(end); ++donotuseme) \
  { \
    std::size_t variable(static_cast<std::size_t>(donotuseme));

  #define END_PARALLEL_FOR }
#else
  #define BEGIN_PARALLEL_FOR(condition,start,end,variable) \
  PRAGMA(omp parallel for if (condition) ) \
  for (std::size_t variable = start; variable < end; ++variable)

  #define END_PARALLEL_FOR
#endif
      

#endif