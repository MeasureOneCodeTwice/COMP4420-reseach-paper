#pragma once

#include <span>

class KFunnel;

class Buffer
{
    private:
        int read_offset;
        int write_offset;

        int k;

        std::span<int> data;

        KFunnel* feeder_funnel;

    public:
        // pass in pointer to memory
        // the memory is managed in the funnels, the buffer just adds a layer of abstraction so we use it with methods
        Buffer();

        Buffer(std::span<int> data);

        Buffer(std::span<int> data, int fullness);

        // insert data to back of data array
        void push_back(int val);

        // read value from front of array WITHOUT moving read_offset
        int* peek_front();

        // read value from front of array
        int* read_front();

        void link_feeder_funnel(KFunnel* feeder_funnel);

        void fill();

        int size();

        bool is_empty();

        bool is_full();

        void print();

        std::span<int> get_contents();

        ~Buffer();
};
