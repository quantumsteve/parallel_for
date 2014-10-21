//#include <omp.h>
#include "tbb/tbb.h"
#include <vector>
#include <iostream>
#include <random>
#include <chrono>

void print(const std::vector<double>& v) {
    for (std::size_t i = 0; i < 10; ++i)
        std::cout << v[i] << '\t';
    std::cout << std::endl;
}

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
    std::size_t length(2000);
    std::vector<std::vector<double>> a(length, std::vector<double>(length)),
                                     b(length, std::vector<double>(length)),
                                     c(length, std::vector<double>(length)),
                                     d(length, std::vector<double>(length));

    
    fillMatrix(a);
    fillMatrix(b);
    
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < length; ++i)
    {
        for (std::size_t j = 0; j < length; ++j)
        {
            for (std::size_t k = 0; k < length; ++k)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    
    for (std::size_t i = 0; i < 10; ++i)
    {
        print(c[i]);
    }
    
    t1 = std::chrono::high_resolution_clock::now();
    tbb::parallel_for(std::size_t(0),std::size_t(length), std::size_t(1),[&](std::size_t i)
    {
        for (std::size_t j = 0; j < length; ++j)
        {
            for (std::size_t k = 0; k < length; ++k)
            {
                d[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    );
    
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::endl;
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;

    for (std::size_t i = 0; i < 10; ++i)
    {
        print(d[i]);
    }
    
    return 0;
}