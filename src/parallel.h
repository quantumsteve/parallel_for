#include <stdint.h>

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define THREADSAFE(...) GET_MACRO(__VA_ARGS__, TS3, TS2, TS1)(__VA_ARGS__)

#define TS1(ws1) !ws1 || ws1->threadSafe()

#define TS2(ws1, ws2) (!ws1 || ws1->threadSafe()) && (!ws2 || ws2->threadSafe())

#define TS3(ws1, ws2, ws3)                                                     \
  (!ws1 || ws1->threadSafe()) && (!ws2 || ws2->threadSafe()) &&                \
      (!ws3 || ws3->threadSafe())

// The syntax used to define a pragma within a macro is different on windows and
// GCC
#ifdef _MSC_VER
#define PRAGMA __pragma
#else //_MSC_VER
#define PRAGMA(x) _Pragma(#x)
#endif //_MSC_VER

/*
   1. HAVE_TBB         - 3rdparty library, should be explicitly enabled
   2. HAVE_OPENMP      - integrated to compiler, defined iff HAVE_TBB is not
   defined and _OPENMP is defined
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

/* ==================== parallel_for ==================== */

enum execution_policy { seq, par, par_vec };

template <class func>
void parallel_for(execution_policy policy, std::size_t start, std::size_t end,
                  func body) {
  if (policy) {
#ifdef HAVE_TBB
    tbb::parallel_for(start, end, body);
#elif(defined(HAVE_OPENMP) && defined(_MSC_VER))
    PRAGMA(omp parallel for)
    for (int64_t i = start; i < end; ++i)
      body(static_cast<std::size_t>(i));
#else
    PRAGMA(omp parallel for)
    for (std::size_t i = start; i < end; ++i)
      body(i);
#endif
  } else {
    for (std::size_t i = start; i < end; ++i)
      body(i);
  }
}
