#include "../include/funnelsort.h"
#include "../include/testlib.h"

#include <cstdlib>
#include <span>
#include <vector>
#include <cmath>
#include <assert.h>
#include <limits.h>

//used by testlib
int tests_passed = 0;
int tests_failed = 0;


bool is_sorted(std::span<int> span)
{
    bool sorted = true;
    for(int i = 1; i < span.size() && sorted; i++)
    {
        sorted = sorted && span[i-1] <= span[i];
    }
    return sorted;
}

std::span<int> populate_random(std::span<int> span, int num_elements)
{
    std::span<int> subspan = span.first(num_elements);
    for(int i = 0; i < subspan.size(); i++)
    {
        subspan[i] = (int)(rand() % 25);
    }
    return subspan;
}

void test_sorts(std::span<int> input) {

    funnel_sort(input);
    verify(is_sorted(input), "");
}

int main(int argc, char *argv[])
{
    std::vector<int> input_buffer;
    input_buffer = std::vector({5,8,3,2,7,1,9,4,0,1});
    test_sorts(input_buffer);
    
    input_buffer.clear();
    input_buffer = std::vector({9,8,7,6,5,4,3,2,1});
    test_sorts(input_buffer);

    input_buffer.clear();
    input_buffer = std::vector({0});
    test_sorts(input_buffer);

    input_buffer.clear();
    test_sorts(std::span<int>(input_buffer).first(0));

    input_buffer.clear();
    input_buffer = std::vector({1, 1, 1});
    test_sorts(input_buffer);

    // --- randomized data ---
    const int INPUT_BUFFER_SIZE = INT_MAX / 2048;
    input_buffer = std::vector<int>(INPUT_BUFFER_SIZE); 

    test_sorts(populate_random(input_buffer, 216));
    test_sorts(populate_random(input_buffer, 9));
    test_sorts(populate_random(input_buffer, 12));
    test_sorts(populate_random(input_buffer, 18));

    int exponent = 1;
    int input_size; 
    while( (input_size = std::pow(2, exponent)) < INPUT_BUFFER_SIZE)
    {
        test_sorts(populate_random(input_buffer, input_size));
        exponent++;
    }
   
    return summary(tests_passed, tests_failed);
}
