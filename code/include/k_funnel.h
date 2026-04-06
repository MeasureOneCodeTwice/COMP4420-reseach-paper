#pragma once

#include "buffer.h"
#include "k_funnel_arenas.h"

#include <cmath>


class KFunnel
{
    private:
        static const int BASE_K = 2;

        int rootk; 
        int k;

        // Variables & functions to check if memory allocation was correct.
        int buffer_space_allocated = 0;
        int num_buffers_allocated = 0;
        int funnels_allocated = 0;

        int total_buffer_space_allocated();
        int total_funnels_allocated();
        int total_buffers_allocated();


        // parent owns the input and output buffers
        std::span<Buffer> input_buffers;
        Buffer* output_buffer;
        
        // this funnel owns the bottom funnels, top funnel, and the buffers connecting the bottom funnels to top funnels
        KFunnel* top_funnel;
        std::span<KFunnel> bottom_funnels;
        std::span<int> middle_buffers_memory;
        std::span<Buffer> middle_buffers;

    
        void create_middle_buffers(KFunnelArenas arenas);
        void create_bottom_funnels(KFunnelArenas arenas);
        void link_buffer_feeders();


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
        inline int middle_buffers_total_memory() {
            return this->num_middle_buffers() * this->middle_buffer_size();
        }


    public:
        KFunnel(
                int k,
                std::span<Buffer> input_buffers, 
                Buffer* output_buffer, 
                KFunnelArenas arenas 
        );

        //need default constructor to be able to create funnel arena.
        //must use other constructor on instances you plan to use.
        KFunnel();

        static int total_buffer_space_required(int k);
        static int total_buffers_required(int k);
        static int total_funnels_required(int k);

        int* min_val_from_inputs();


        ~KFunnel();
};
