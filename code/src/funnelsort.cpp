#include "../include/funnelsort.h"
#include "../include/k_funnel.h"

#include <cstdlib>
#include <iostream>
#include <span>
#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <limits.h>

int compute_k(int n)
{
    return std::ceil( std::cbrt(n) );
}

//split the input buffer into k sorted input spans
std::span<Buffer> create_input_buffers(std::span<int> input, KFunnelArenas arenas)
{
    int k = compute_k(input.size());

    // allocate k input buffers
    std::span<Buffer> buffers = arenas.buffer_arena->allocate(k);
    int ideal_buffer_size = std::ceil( (double)input.size() / k );
    int num_allocated_items = 0;
    int tot_buffer_size = 0;

    for(int i = 0; i < k; i++)
    {
        int buffer_size = std::min(ideal_buffer_size, (int)input.size() - num_allocated_items);
        std::span<int> buffer_data = input.subspan(num_allocated_items, buffer_size);
        num_allocated_items += buffer_size;

        // each of the k inputs must be sorted
        // but...each input buffer could be huge
        // por que no mas recursiva ??
        // yes, we're doing funnelsort all the way down
        if (buffer_size > MAX_INPUT_BUFF_WO_RECURSION)
        {
            funnel_sort(buffer_data);
        }
        else
        {
            std::sort(std::begin(buffer_data), std::end(buffer_data));
        }

        std::span<int> buffer_space = arenas.buffer_space_arena->allocate(buffer_size);
        std::copy(std::begin(buffer_data), std::end(buffer_data), std::begin(buffer_space));

        buffers[i] = Buffer(buffer_space, buffer_size);
    }
    return std::span(buffers);
}

void funnel_sort(std::span<int> to_sort, bool printDebugInfo)
{
    const int k = compute_k(to_sort.size());
    if(printDebugInfo)
    {
        Buffer* to_sort_for_print = new Buffer(to_sort); // just to print
        std::cout << "\nfunnel: n = " << to_sort.size() << std::endl;
        std::cout << "funnel : k = " << k << std::endl;
        std::cout << "Raw to_sort: \n";
        to_sort_for_print->print();
        std::cout << "\n";
        delete to_sort_for_print;
    }


    //need additional space for to_sort and output buffers
    /* MemoryArena<int> buffer_space_arena (); */
    MemoryArena<int> buffer_space_arena ( KFunnel::total_buffer_space_required(k) + to_sort.size() * 2 );
    MemoryArena<Buffer> buffer_arena ( KFunnel::total_buffers_required(k) + k + 1);
    MemoryArena<KFunnel> funnel_arena ( KFunnel::total_funnels_required(k) + 1 );
    KFunnelArenas arenas { 
        buffer_space_arena: &buffer_space_arena, 
        buffer_arena: &buffer_arena,
        funnel_arena: &funnel_arena
    };

    Buffer* output_buffer = &(arenas.buffer_arena->allocate(1)[0]);
    *output_buffer = Buffer(arenas.buffer_space_arena->allocate(to_sort.size()));

    std::span<Buffer> to_sort_buffers = create_input_buffers(to_sort, arenas);

    
    std::span<KFunnel> funnel = arenas.funnel_arena->allocate(1);
    funnel[0] = KFunnel(k, to_sort_buffers, output_buffer, arenas);

    output_buffer->fill();

    const std::span<int> output_buffer_contents = output_buffer->get_contents();
    std::vector<int> result(output_buffer_contents.size()) ;
    std::copy(
        std::begin(output_buffer_contents),
        std::end(output_buffer_contents),
        std::begin(to_sort)
    );

    if(printDebugInfo)
    {
        std::cout << "\nOutput:\n";
        output_buffer->print();
        std::cout << "\n\n";
    }
}


