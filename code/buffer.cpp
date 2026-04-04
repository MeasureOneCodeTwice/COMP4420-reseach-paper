
#include "buffer.h"
#include "k_funnel.h"

#include <iostream>

// pass in pointer to memory
// the memory is managed in the funnels, the buffer just adds a layer of abstraction so we use it with methods
Buffer::Buffer(int capacity, int* data) : Buffer(capacity, 0, data){}

Buffer::Buffer(int capacity, int size, int* data)
{
    this->capacity = capacity;
    this->read_offset = 0;
    this->write_offset = size;
    this->data = data;
    this->feeder_funnel = nullptr;
}

// insert data to back of data array
bool Buffer::push_back(int val)
{
    bool success = false;

    // reset offsets if the array is empty
    if (this->is_empty())
    {
        this->read_offset = 0;
        this->write_offset = 0;
    }

    // only write if there's space at the back of this buffer
    if (this->capacity > this->write_offset)
    {
        data[write_offset] = val;
        write_offset++;
        success = true;
    }
    return success;
}

// read value from front of array WITHOUT moving read_offset
int* Buffer::peek_front()
{
    int* val = nullptr;
    if (!this->is_empty())
    {
        val = &(this->data[this->read_offset]);
    }
    return val;
}

// read value from front of array
int* Buffer::read_front()
{
    int* val = nullptr;
    if (!this->is_empty())
    {
        val = &(this->data[this->read_offset]);
        this->read_offset++;
    }
    return val;
}

void Buffer::link_feeder_funnel(KFunnel* feeder_funnel)
{
    this->feeder_funnel = feeder_funnel;
}

void Buffer::fill()
{
    if(this->feeder_funnel == nullptr)
    {
        // can't fill this one, it has no feeder
        // must be the top level funnel input
        return;
    }

    bool done = false;
    while(!this->is_full() && !done)
    {
        int* min = this->feeder_funnel->min_val_from_inputs();

        done = min == nullptr; // feeder funnel could be drained
        
        if(!done)
        {
            this->push_back(*min);
        }
    }
}

int Buffer::size()
{
    return this->write_offset - this->read_offset;
}

bool Buffer::is_empty()
{
    return this->size() <= 0;
}

bool Buffer::is_full()
{
    return this->size() >= this->capacity;
}

void Buffer::print()
{
    std::cout << "[";
    for(int i = 0; i < this->capacity; i++)
    {
        std::cout << this->data[i];

        if(i < this->capacity - 1)
        {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

Buffer::~Buffer()
{
    // nothing on the heap
}
