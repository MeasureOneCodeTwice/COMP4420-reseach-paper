#include "k_funnel.h"
#include "buffer.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <limits.h>

const int MAX_INPUT_BUFF_WO_RECURSION = INT_MAX;

void funnel_sort(int n, int* input, int* output, bool print_buffers = true);

bool assert_sorted(int size, int* arr)
{
    bool sorted = true;
    for(int i = 1; i < size; i++)
    {
        sorted = sorted && arr[i-1] <= arr[i];
    }
    return sorted;
}

int compute_k(int n)
{
    return std::ceil( std::cbrt(n) );
}

std::vector<Buffer*> create_input_buffers(int n, int* input, bool print_buffers = true)
{
    int k = compute_k(n);
    // create vector of k input buffers
    std::vector<Buffer*> input_buffers;
    int input_sizes = std::ceil( (double)n / k );

    for(int i = 0; i < k; i++)
    {
        int offset = i * input_sizes;        
        int buffer_size = std::min(input_sizes, n - offset);

        Buffer* buff = nullptr;

        if (buffer_size > 0)
        {
            int* start_ptr = &input[offset];
            
            // each of the k inputs must be sorted
            // but...each input buffer could be huge
            // por que no mas recursiva ??
            // yes, we're doing funnelsort all the way down
            if (buffer_size > MAX_INPUT_BUFF_WO_RECURSION)
            {
                int* input_slice_copy = new int[buffer_size];
                std::copy(start_ptr, start_ptr + buffer_size, input_slice_copy);

                funnel_sort(buffer_size, input_slice_copy, start_ptr, print_buffers);

                delete[] input_slice_copy;
            }
            else
            {
                std::sort(start_ptr, start_ptr + buffer_size);
            }

            buff = new Buffer(buffer_size, buffer_size, start_ptr);
        }
        else
        {
            buff = new Buffer(0, 0, nullptr);
        }
        input_buffers.push_back(buff);

        if(print_buffers)
        {
            std::cout << "\tInput Buffer [" << i << "]\n\t";
            buff->print();
        }
    }
    return input_buffers;
}

void funnel_sort(int n, int* input, int* output, bool print_buffers)
{
    Buffer* output_buffer = new Buffer(n, output);
    std::vector<Buffer*> input_buffers = create_input_buffers(n, input, print_buffers);
    
    if(print_buffers)
    {
        Buffer* input_for_print = new Buffer(n, input); // just to print
        std::cout << "\nfunnel: n = " << n << std::endl;
        std::cout << "funnel : k = " << compute_k(n) << std::endl;
        std::cout << "Raw Input: \n";
        input_for_print->print();
        std::cout << "\n";
        delete input_for_print;
    }
    KFunnel* k_funnel = new KFunnel(compute_k(n), input_buffers, output_buffer);

    output_buffer->fill();

    if(print_buffers)
    {
        std::cout << "\nOutput:\n";
        output_buffer->print();
        std::cout << "\n\n";
    }

    // cleanup
    k_funnel->print_stats();
    delete k_funnel;
    delete output_buffer;
    for(int i = 0; i < input_buffers.size(); i++)
    {
        delete input_buffers[i];
    }
}

void populate_random(int n, int* arr)
{
    for(int i = 0; i < n; i++)
    {
        arr[i] = (int)(rand() % 25);
    }
}

void test_my_input(int size, int* input, int test_num, bool print_buffers = true)
{
    std::cout << "=== Test " << test_num << " ===========================\n";
    std::cout << "\nn = " << size << std::endl;
    std::cout << "k = " << compute_k(size) << std::endl;

    const int n = size;
    int* output = new int[n]; // heap instead of stack

    funnel_sort(n, input, output, print_buffers);

    std::cout << "Sorted : " << (assert_sorted(size, output) ? "PASSED" : "FAILED") << std::endl;
    std::cout << "\n\n";

    delete[] output;
}

void test_random_input(int size, int test_num, bool print_buffers = true)
{
    const int n = size;
    int* input = new int[n]; // heap instead of stack
    populate_random(n, input);

    test_my_input(size, input, test_num, print_buffers);

    delete[] input;
}

int main(int argc, char *argv[])
{
    int test_num = 1;

    /* int in[] = {5,8,3,2,7,1,9,4,0,1}; */
    /* test_my_input(sizeof(in) / sizeof(int), in, test_num++); */

    /* int in2[] = {9,8,7,6,5,4,3,2,1}; */
    /* test_my_input(sizeof(in2) / sizeof(int), in2, test_num++); */

    /* int in3[] = {0}; */
    /* test_my_input(sizeof(in3) / sizeof(int), in3, test_num++); */

    /* int in4[] = {}; */
    /* test_my_input(sizeof(in4) / sizeof(int), in4, test_num++); */

    /* int in5[] = {1,1,1}; */
    /* test_my_input(sizeof(in5) / sizeof(int), in5, test_num++); */

    test_random_input(8, test_num++);
    test_random_input(9, test_num++);
    test_random_input(12, test_num++);
    test_random_input(18, test_num++);
    test_random_input(128, test_num++, false);
    test_random_input(512, test_num++, false);
    test_random_input(1024, test_num++, false);
    test_random_input(1024, test_num++, false);

    test_random_input(4096, test_num++, false);

    // test_random_input(std::pow(2, 32), test_num++, false);

    // for(int i = 10; i < 34; i++)
    // {
    //     test_random_input(std::pow(2, i), test_num++, false);
    // }
    
    return EXIT_SUCCESS;
}
