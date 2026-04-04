#pragma once

#include "buffer.h"

#include <vector>

class KFunnel
{
    private:
        const int BASE_K = 2;

        int rootk; 
        int k;

        // parent funnel owns the input and output buffers
        std::vector<Buffer*> input_buffers;
        Buffer* output_buffer;
        
        // this funnel owns the bottom funnels, top funnel, and the buffers connecting the bottom funnels to top funnels
        std::vector<KFunnel*> bottom_funnels;
        int* middle_buffers_memory;
        std::vector<Buffer*> middle_buffers;
        KFunnel* top_funnel;

        void setup_middle_buffers();

        void setup_bottom_funnels();

        void link_buffer_feeders();

    public:
        KFunnel(int k, std::vector<Buffer*> input_buffers, Buffer* output_buffer);

        int* min_val_from_inputs();

        ~KFunnel();
};
