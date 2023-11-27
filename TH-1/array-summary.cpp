/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <string.h>
#include <chrono>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

struct arguments
{
    int start;
    int end;
    int *arr;
};

void* partialSum(void* arg)
{
    arguments* part = (arguments*) arg;
    
    int sum = 0;
    
    for(int i = part->start; i < part->end; ++i)
    {
        sum += part->arr[i];
    }

    return new int(sum);
}

int sumWithoutThreading(int *arr, int size)
{
    int sum = 0;
    
    for(int i = 0; i < size; i++)
    {
        sum += arr[i];
    }
    
    return sum;
}

int sumWithThreading(int *arr, int size, int threadsCount)
{
    pthread_t * threads =  new pthread_t[threadsCount];
    arguments** argArr =  new arguments * [threadsCount];
    
    int sum = 0;
    
    for(int i = 0; i < threadsCount; i++)
    {
        argArr[i] = new arguments;
        argArr[i]->arr = arr;
        argArr[i]->start = i*(size/threadsCount);
        argArr[i]->end = min(argArr[i]->start + (size / threadsCount), size);
        
        int result = pthread_create(&threads[i], NULL, partialSum, argArr[i]);
        if (result != 0)
        {
            cerr << strerror(result) << std::endl;
            exit(result);
        }
    }
    
    for(int i = 0; i < threadsCount; i++)
    {
        void* return_value;
        
        int result = pthread_join(threads[i], &return_value);
        int* sum_result = (int*)return_value;
        
        sum += *sum_result;
        
        delete sum_result;
        delete argArr[i];
    }
    
    delete[] argArr; 
    
    return sum;
}

int main(int argc, char** argv)
{
    int size = stoi(argv[1]);
    int threadsCount = stoi(argv[2]);
    int* arr = new int[size];
    
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }

    struct tms start, end;
    clock_t start2, end2;
    times(&start);
    start2 = times(&start);
    sumWithoutThreading(arr, size);
    times(&end);
    end2 = times(&end);

    clock_t ticks_per_second = sysconf(_SC_CLK_TCK);
    double task_duration = (double)(end2 - start2) / ticks_per_second;
    cout << "Time spent without threads: " << task_duration << endl;
   

    times(&start);
    clock_t start3, end3;
    start3 = times(&start);
    sumWithThreading(arr, size, threadsCount);
    times(&end);
    end3 = times(&end);
    double task_duration2 = (double)(end3 - start3) / ticks_per_second;
    cout << "Time spent with " << threadsCount << " threads: " << task_duration2;

    delete[] arr;

    return 0;
}
