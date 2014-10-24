#include <iostream>
#include <list>
#include <tbb/task.h>
#include <tbb/task_group.h>
#include "tbb/task_scheduler_init.h"

using namespace tbb;


long fib(long a)
{
    if (a < 2) return 1;
    
    return fib(a - 1) + fib(a - 2);
}

class PrintTask
{
public:
    void operator()(int i)
    {
        fib(i);
    }
};

int main()
{
    tbb::task_scheduler_init init; //creates 4 threads
    task_group group;
    
    
    for (int i = 3; i < 2000000; ++i)
    {
        group.run(std::bind(PrintTask(),i));
        //std::cout << i << std::endl;
    }
    
    std::cout << "done" << std::endl;
    group.wait();
    
    return(0);
}