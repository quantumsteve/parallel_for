#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "parallel.h"

static void fillMatrix( std::vector<std::vector<double>>& matrix)
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

    std::cout << "serial old: " << std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    for (std::size_t i=0;i<length;++i)
    {
        for (std::size_t j = 0; j < length; ++j)
        {
            for (std::size_t k = 0; k < length; ++k)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    
    return 0;
}

