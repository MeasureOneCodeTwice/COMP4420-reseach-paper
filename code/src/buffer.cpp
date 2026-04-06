#include "../include/buffer.h"
#include "../include/k_funnel.h"

#include <assert.h>
#include <stdexcept>
#include <span>
#include <iostream>

// pass in pointer to memory
// the memory is managed in the funnels, the buffer just adds a layer of abstraction so we use it with methods
Buffer::Buffer() : Buffer(std::span<int>(), 0) {}
Buffer::Buffer(std::span<int> data) : Buffer(data, 0) {}
Buffer::Buffer(std::span<int> data, int fullness) 
{
    this->read_offset = 0;
    this->write_offset = fullness;
    this->data = data;
    this->feeder_funnel = nullptr;
}

// insert data to back of data array
void Buffer::push_back(int val)
{
    // reset offsets if the array is empty
    if (this->is_empty())
    {
        this->read_offset = 0;
        this->write_offset = 0;
    }

    // only write if there's space at the back of this buffer
    if (this->data.size() <= this->write_offset)
    {
        throw std::out_of_range("Not enough buffer space.");
    }

    data[write_offset] = val;
    write_offset++;
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
        // must be a bottom level funnel
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
    const int size = this->write_offset - this->read_offset;
    if(size < 0) {
        std::cout << (this->data.size()) << std::endl;
        std::cout << "this->write_offset:" << this->write_offset << std::endl; 
        std::cout << "this->read_offset:" << this->read_offset << std::endl; 
    }
    return size;
}

bool Buffer::is_empty()
{
    assert(this->size() >= 0);
    return this->size() == 0;
}

bool Buffer::is_full()
{
    return this->size() >= this->data.size();
}

void Buffer::print()
{
    std::cout << "[";
    for(int i = 0; i < this->data.size(); i++)
    {
        std::cout << this->data[i];

        if(i < this->data.size() - 1)
        {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

std::span<int> Buffer::get_contents() {
    return  this->data.subspan(this->read_offset, this->size());
}

Buffer::~Buffer()
{
    //We don't own the memory backing the buffer
}
