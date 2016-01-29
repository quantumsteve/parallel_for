#include <algorithm>
#include <stdint.h>
#include <boost/iterator/counting_iterator.hpp>

/*
 1. USE_TBB         - 3rdparty library, should be explicitly enabled
 2. USE_OPENMP      - integrated to compiler, defined iff HAVE_TBB is not
 defined and _OPENMP is defined
 3. USE_SERIAL
 */

#if defined USE_TBB
#define PARALLEL_FRAMEWORK "tbb"
#include "tbb/tbb.h"
#elif defined(_OPENMP) && defined(USE_OPENMP)
#define PARALLEL_FRAMEWORK "openmp"
#include <omp.h>
#else
#define PARALLEL_FRAMEWORK "serial"
#endif // PARALLEL_FRAMEWORK

/* ===================== ThreadSafe ===================== */

template <typename Arg> inline bool ThreadSafe(const Arg workspace) {
  return workspace->threadSafe();
}

template <typename Arg, typename... Args>
inline bool ThreadSafe(const Arg &workspace, const Args &... others) {
  return workspace->threadSafe() && ThreadSafe(others...);
}

/* ==================== parallel_for_each ==================== */

enum mantid_threading_library { TBB, OPENMP, NONE };
enum execution_policy : bool { seq, par }; // parallel stl also includes par_vec

template <mantid_threading_library type, typename func>
inline typename std::enable_if<type == NONE>::type
parallel_for_each(execution_policy /*policy*/, std::size_t start, std::size_t end,
                  func body) {
  std::for_each(boost::counting_iterator<std::size_t>(start),
                boost::counting_iterator<std::size_t>(end), body);
}

#if defined USE_TBB

template <mantid_threading_library type, typename func>
inline typename std::enable_if<type == TBB>::type
parallel_for_each(execution_policy policy, std::size_t start, std::size_t end,
                  func body) {
  if (policy) {
    tbb::parallel_for(start, end, body);
  } else {
    parallel_for_each<NONE>(seq, start, end, body);
  }
}

template <typename... Args> inline void parallel_for_each(Args &&... args) {
  parallel_for_each<TBB>(std::forward<Args>(args)...);
}

#elif USE_OPENMP

template <mantid_threading_library type, typename func>
inline typename std::enable_if<type == OPENMP>::type
parallel_for_each(execution_policy policy, std::size_t start, std::size_t end,
                  func body) {
#ifdef _MSC_VER
#pragma omp parallel for if (policy)
  for (int64_t i = start; i < end; ++i)
    body(static_cast<std::size_t>(i));
#else
#pragma omp parallel for if (policy)
  for (std::size_t i = start; i < end; ++i)
    body(i);
#endif
}

template <typename... Args> inline void parallel_for_each(Args &&... args) {
  parallel_for_each<OPENMP>(std::forward<Args>(args)...);
}

#else

template <typename... Args> inline void parallel_for(Args &&... args) {
  parallel_for_each<NONE>(std::forward<Args>(args)...);
}

#endif
