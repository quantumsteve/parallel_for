#include <iostream>
#include "parallel.h"

class SafeWS
{
public:
    bool threadSafe() const { return true; }
};

class UnsafeWS
{
public:
    bool threadSafe() const { return false; }
};

int main()
{
    auto ws4 = std::make_shared<SafeWS>();
    auto ws5 = std::make_shared<UnsafeWS>();
    
    bool test1 = THREADSAFE(ws4);
    bool test2 = THREADSAFE(ws5);
    bool test3 = THREADSAFE(ws4,ws4);
    bool test4 = THREADSAFE(ws4,ws5);
    bool test5 = THREADSAFE(ws4,ws4,ws4);
    bool test6 = THREADSAFE(ws4,ws4,ws5);
    
    std::cout << test1 << " " << test3 << " " << test5 <<  std::endl;
    std::cout << test2 << " " << test4 << " " << test6 <<  std::endl;

    return 0;
}