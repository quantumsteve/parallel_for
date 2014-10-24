#include <iostream>
#include "parallel.h"

int main()
{
    Mutex mutex;
    std::size_t length(10);
    BEGIN_PARALLEL_FOR(true,0,length,i)
    {
        Mutex::ScopedLock lock(mutex);
        std::cout << "Hello from task " << i << std::endl;
    }
    END_PARALLEL_FOR
    
    return 0;
}
