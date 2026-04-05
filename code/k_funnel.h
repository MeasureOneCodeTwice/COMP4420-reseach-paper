#pragma once

#include "buffer.h"

#include <vector>
#include <cmath>


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

        void create_middle_buffers();

        void create_bottom_funnels();

        void link_buffer_feeders();

        int MEMORY_ALLOCATED = 0;
        int FUNNELS_ALLOCATED = 0;
        int BUFFERS_ALLOCATED = 0;

        inline int middle_buffer_size() {
            return std::ceil( std::pow(this->rootk, 3) );
        }
        inline int num_middle_buffers() {
            return this->rootk;
        }
        inline int num_bottom_funnels() {
            return this->rootk;
        }
        inline int bottom_funnel_size() {
            return this->rootk;
        }
        inline int top_funnel_size() {
            return this->rootk;
        }


    public:
        KFunnel(int k, std::vector<Buffer*> input_buffers, Buffer* output_buffer);

        int* min_val_from_inputs();

        void print_stats();

        int memory_allocated();
        int funnels_allocated();
        int buffers_allocated();

        ~KFunnel();
};
