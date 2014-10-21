#include <cmath>
#include <stdint.h>
#include <iostream>
#include <vector>
#if defined __linux__ || defined __APPLE__
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/types.h>
#endif

#ifdef _OPENMP
    #define HAVE_OPENMP
#endif

#ifdef __APPLE__
    #define HAVE_GCD
#endif

/* IMPORTANT: always use the same order of defines
   1. HAVE_TBB         - 3rdparty library, should be explicitly enabled
   3. HAVE_OPENMP      - integrated to compiler, should be explicitly enabled
   4. HAVE_GCD         - system wide, used automatically        (APPLE only)
*/

#if defined HAVE_TBB
    #include "tbb/tbb_stddef.h"
    #if TBB_VERSION_MAJOR*100 + TBB_VERSION_MINOR >= 202
        #include "tbb/tbb.h"
        #include "tbb/task.h"
        #if TBB_INTERFACE_VERSION >= 6100
            #include "tbb/task_arena.h"
        #endif
        #undef min
        #undef max
    #else
        #undef HAVE_TBB
    #endif // end TBB version
#endif

#ifndef HAVE_TBB
    #if defined HAVE_OPENMP
        #include <omp.h>
    #elif defined HAVE_GCD
        #include <dispatch/dispatch.h>
        #include <pthread.h>
    #endif
#endif

#if defined HAVE_TBB && TBB_VERSION_MAJOR*100 + TBB_VERSION_MINOR >= 202
#  define PARALLEL_FRAMEWORK "tbb"
#elif defined HAVE_OPENMP
#  define PARALLEL_FRAMEWORK "openmp"
#elif defined HAVE_GCD
#  define PARALLEL_FRAMEWORK "gcd"
#endif

//////////////////////////////// Range /////////////////////////////////
/*!
 The 2D range class
 This is the class used to specify a continuous subsequence, i.e. part of a contour, or a column span in a matrix.
 */
class Range
{
public:
    Range() : start(0), end(0) {}
    Range(int _start, int _end) : start(_start), end(_end) {}
    int size() const { return end - start; }
    bool empty() const;
    int start, end;
};

// a base body class
class ParallelLoopBody
{
public:
    virtual ~ParallelLoopBody();
    virtual void operator() (const Range& range) const = 0;
};

ParallelLoopBody::~ParallelLoopBody() {}

#ifdef PARALLEL_FRAMEWORK
    class ParallelLoopBodyWrapper
    {
    public:
        ParallelLoopBodyWrapper(const ParallelLoopBody& _body, const Range& _r)
        {
            body = &_body;
            wholeRange = _r;
            double len = wholeRange.end - wholeRange.start;
        }
        void operator()(const Range& sr) const
        {
            Range r;
            r.start = wholeRange.start;
            r.end = wholeRange.end;
            (*body)(r);
        }
    protected:
        const ParallelLoopBody* body;
        Range wholeRange;
    };

#if defined HAVE_TBB
    class ProxyLoopBody : public ParallelLoopBodyWrapper
    {
    public:
        ProxyLoopBody(const ParallelLoopBody& _body, const Range& _r)
        : ParallelLoopBodyWrapper(_body, _r)
        {}
        void operator ()(std::size_t i) const
        {
            this->ParallelLoopBodyWrapper::operator()(Range(i,i+1));
        }
    };
#elif defined HAVE_OPENMP
    typedef ParallelLoopBodyWrapper ProxyLoopBody;
#elif defined HAVE_GCD
    typedef ParallelLoopBodyWrapper ProxyLoopBody;
    static void block_function(void* context, size_t index)
    {
        ProxyLoopBody* ptr_body = static_cast<ProxyLoopBody*>(context);
        (*ptr_body)(Range(index, index + 1));
    }
#else
    typedef ParallelLoopBodyWrapper ProxyLoopBody;
#endif

static int numThreads = -1;

#if defined HAVE_TBB
static tbb::task_scheduler_init tbbScheduler(tbb::task_scheduler_init::deferred);
#elif defined HAVE_OPENMP
static int numThreadsMax = omp_get_max_threads();
#elif defined HAVE_GCD
// nothing for GCD
#endif

#endif // PARALLEL_FRAMEWORK

/* ================================   parallel_for_  ================================ */

void parallel_for_(std::size_t start, std::size_t end, const ParallelLoopBody& body)
{
#ifdef PARALLEL_FRAMEWORK
    if(numThreads != 0)
    {
        ProxyLoopBody pbody(body, Range(start,end));
        if( end - start == 1 )
        {
            body(Range(start,end));
            return;
        }

#if defined HAVE_TBB

        tbb::parallel_for(start, end, pbody);

#elif defined HAVE_OPENMP

        #pragma omp parallel for schedule(dynamic)
        for (int i = start; i < end; ++i)
            pbody(Range(i, i + 1));

#elif defined HAVE_GCD

        dispatch_queue_t concurrent_queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        dispatch_apply_f(end - start, concurrent_queue, &pbody, block_function);

#else

#error You have hacked and are compiling with an unsupported parallel framework

#endif

    }
    else

#endif // PARALLEL_FRAMEWORK
    {
        body(Range(start,end));
    }
}

//header
class ParallelClass: public ParallelLoopBody
{
public:
    double* m1;
    double* m2;
    double* result;
    ParallelClass();
    void operator()( const Range &r )const;
private:
};

ParallelClass::ParallelClass(){}

void ParallelClass::operator()( const Range & r ) const
{
    // loop for the range
    for( int i = r.start ; i < r.end ; ++i )
    {
        result[i] = m1[i] + m2[i];
    }
}

const char* currentParallelFramework() {
#ifdef PARALLEL_FRAMEWORK
    return PARALLEL_FRAMEWORK;
#else
    return NULL;
#endif
}


void Print(const std::vector<double>& v) {
    for(auto& i : v)
        std::cout << i << '\n';
}


int main()
{
    std::vector<double> v1(100,1.0),v2(100,2.0),v3(100,0.0);
    std::cout << currentParallelFramework() << std::endl;
    ParallelClass sum;
    sum.m1 = &v1[0];
    sum.m2 = &v2[0];
    sum.result = &v3[0];
    parallel_for_(0,100, sum);
    Print(v3);
    return 0;
}

