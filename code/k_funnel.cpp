
#include "k_funnel.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <assert.h>


void KFunnel::create_middle_buffers()
{
    int middle_buffers_total_space = this->num_middle_buffers() * this->middle_buffer_size();
    this->middle_buffers_memory = new int[middle_buffers_total_space];
    MEMORY_ALLOCATED += middle_buffers_total_space;

    for(int i = 0; i < this->num_middle_buffers(); i++)
    {
        int offset = i * this->middle_buffer_size();
        this->middle_buffers.push_back(new Buffer(this->middle_buffer_size(), &this->middle_buffers_memory[offset]));
        BUFFERS_ALLOCATED++;
    }
}

//"where the buffers are don't matter, as long as each funnel is laid out contigiously"
void KFunnel::create_bottom_funnels()
{
    int input_buffers_allocated = 0;
    int input_buffers_available = this->input_buffers.size(); 
    for(int i = 0; i < this->num_bottom_funnels(); i++)
    {
        // may not have a perfect number of input buffers for each bottom funnel, 
        // last funnel may have fewer inputs
        int num_buffers_for_funnel = std::min(
                input_buffers_available,
                this->bottom_funnel_size()

        );


        //TODO: Find a better way to do this.
        std::vector<Buffer*> funnel_buffers;
        for(int j = input_buffers_allocated; j < this->input_buffers.size(); j++)
        {
            funnel_buffers.push_back(this->input_buffers[j]);
        }

        bottom_funnels.push_back(
                new KFunnel(this->bottom_funnel_size(), input_buffers, this->middle_buffers[i])
        );

        input_buffers_allocated += num_buffers_for_funnel; 
        input_buffers_available -= num_buffers_for_funnel; 
    }
}

void KFunnel::link_buffer_feeders()
{
    if (k > BASE_K)
    {
        assert(this->middle_buffers.size() == this->num_middle_buffers());
        assert(this->bottom_funnels.size() == this->num_bottom_funnels());
        assert(this->num_bottom_funnels() == this->num_middle_buffers());

        // output_buffer fed by top_funnel
        this->output_buffer->link_feeder_funnel(this->top_funnel);
        // middle_buffers are fed by bottom_funnels
        for(int i = 0; i < this->num_middle_buffers(); i++)
        {
            Buffer* curr_buff = this->middle_buffers[i];
            curr_buff->link_feeder_funnel(this->bottom_funnels[i]);
        }
    }
    else
    {
        // output_buffer fed by this funnel
        this->output_buffer->link_feeder_funnel(this);
    }
}

KFunnel::KFunnel(int k, std::vector<Buffer*> input_buffers, Buffer* output_buffer)
{
    this->k = k;
    this->rootk = std::ceil( std::sqrt(this->k) );

    this->input_buffers = input_buffers;
    this->output_buffer = output_buffer;

    // base case, make it null
    this->top_funnel = nullptr;
    this->middle_buffers_memory = nullptr;

    // recursively create funnels until we hit the base case
    // Base Case Funnel:
    // - BASE_K input buffers, and 1 output buffer
    // Recursive Case Funnel:
    // - rootk-funnels nested inside, 
    //   with middle_buffers connecting top and bottom funnels
    if (k > BASE_K)
    {
        this->create_middle_buffers();

        // inputs: middle_buffers
        // output: output_buffer
        this->top_funnel = new KFunnel(this->top_funnel_size(), this->middle_buffers, this->output_buffer);
        FUNNELS_ALLOCATED++;

        // inputs: input_buffers, rootk input buffs for each funnel
        // output: middle_buffers, one for each funnel
        this->create_bottom_funnels();

    }
    // link all buffers to their feeders
    this->link_buffer_feeders();
}

int* KFunnel::min_val_from_inputs()
{
    int* min_ptr = nullptr;
    Buffer* from_buffer = nullptr;
    for(int i = 0; i < this->input_buffers.size(); i++)
    {
        Buffer* curr_buffer = this->input_buffers[i];

        // Buffers can be size = 0, but not nullptr
        assert(curr_buffer != nullptr);
        
        if (curr_buffer->is_empty())
        {
            // must fill the input buffers first
            curr_buffer->fill();
        }

        // the input buffer should have been filled by now
        if (curr_buffer->is_empty())
        {
            // EXCEPT: in the case where this input buffer and its downstream inputs are all drained
            // could set a flag for this buffer if we wanted...
            // or waste some instructions...
            continue;
        }

        assert(!curr_buffer->is_empty());

        // now find if this buffer has the min val
        int* curr_val_ptr = curr_buffer->peek_front();

        assert(curr_val_ptr != nullptr); // should not be an empty buffer
        
        // init min_ptr if not yet assigned
        if (min_ptr == nullptr)
        {
            min_ptr = curr_val_ptr;
            from_buffer = curr_buffer;
        }
        else if (*curr_val_ptr < *min_ptr)
        {
            // assign the new min
            min_ptr = curr_val_ptr;
            from_buffer = curr_buffer;
        }
    }
    
    if (min_ptr != nullptr)
    {
        // we found a min value from the input buffers
        // actually read it now, moving the read_offset forward on that input buffer
        min_ptr = from_buffer->read_front();
    }
    else
    {
        // all downstream inputs for this funnel must be empty
        // need to stop using this funnel
        // or just waste some instructions every fill stage...
    }
    return min_ptr;
}

KFunnel::~KFunnel()
{
    for (int i = 0; i < this->middle_buffers.size(); i++)
    {
        delete this->middle_buffers[i];
    }
    
    for (int i = 0; i < this->bottom_funnels.size(); i++)
    {
        delete this->bottom_funnels[i];
    }

    if (this->top_funnel != nullptr)
    {
        delete this->top_funnel;
    }

    if (this->middle_buffers_memory != nullptr)
    {
        delete[] this->middle_buffers_memory;
    }
}


int KFunnel::memory_allocated() {
    int memory_allocated = this->MEMORY_ALLOCATED;
    for(int i = 0; i < this->bottom_funnels.size(); i++) {
        memory_allocated += this->bottom_funnels[i]->memory_allocated();
    }
    if(this->top_funnel != nullptr) {
        memory_allocated += this->top_funnel->memory_allocated();
    }
    return memory_allocated;
}
int KFunnel::funnels_allocated() {
    int memory_allocated = this->FUNNELS_ALLOCATED;
    for(int i = 0; i < this->bottom_funnels.size(); i++) {
        memory_allocated += this->bottom_funnels[i]->funnels_allocated();
    }
    if(this->top_funnel != nullptr) {
        memory_allocated += this->top_funnel->funnels_allocated();
    }
    return memory_allocated;
}
int KFunnel::buffers_allocated() {

    int memory_allocated = this->BUFFERS_ALLOCATED;
    for(int i = 0; i < this->bottom_funnels.size(); i++) {
        memory_allocated += this->bottom_funnels[i]->buffers_allocated();
    }
    if(this->top_funnel != nullptr) {
        memory_allocated += this->top_funnel->buffers_allocated();
    }
    return memory_allocated;
}

void KFunnel::print_stats() {

    std::cout << "MEMORY_ALLOCATED: " << this->memory_allocated() << std::endl;
    std::cout << "FUNNELS_ALLOCATED: " << this->funnels_allocated()  << std::endl;
    std::cout << "BUFFERS_ALLOCATED: " << this->buffers_allocated()   << std::endl;

}
