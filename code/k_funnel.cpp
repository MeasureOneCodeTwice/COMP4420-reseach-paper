
#include "k_funnel.h"

#include <vector>
#include <cmath>
#include <assert.h>

void KFunnel::setup_middle_buffers()
{
    int middle_buffer_size = std::ceil( std::pow(this->rootk, 3) );
    int middle_buffers_total_space = middle_buffer_size * rootk;
    this->middle_buffers_memory = new int[middle_buffers_total_space];
    for(int i = 0; i < rootk; i++)
    {
        int offset = i * middle_buffer_size;
        this->middle_buffers.push_back(new Buffer(middle_buffer_size, &this->middle_buffers_memory[offset]));
    }
}

void KFunnel::setup_bottom_funnels()
{
    for(int i = 0; i < this->rootk; i++)
    {
        // may not have a perfect number of input buffers for each bottom funnel, 
        // last funnel may have fewer inputs
        int start_index = i * this->rootk;
        int end_index = std::min( (int)this->input_buffers.size(), (i + 1) * this->rootk);
        std::vector<Buffer*> sliced_input_buffers;

        for(int j = start_index; j < end_index; j++)
        {
            sliced_input_buffers.push_back(this->input_buffers[j]);
        }

        bottom_funnels.push_back(new KFunnel(this->rootk, sliced_input_buffers, this->middle_buffers[i]));
    }
}

void KFunnel::link_buffer_feeders()
{
    if (k > BASE_K)
    {
        // output_buffer fed by top_funnel
        this->output_buffer->link_feeder_funnel(this->top_funnel);
    
        // middle_buffers are fed by bottom_funnels
        assert(this->middle_buffers.size() == this->bottom_funnels.size());
        for(int i = 0; i < this->middle_buffers.size(); i++)
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
        // create middle buffers
        this->setup_middle_buffers();

        // create top funnel
        // inputs: middle_buffers
        // output: output_buffer
        this->top_funnel = new KFunnel(this->rootk, this->middle_buffers, this->output_buffer);

        // create bottom funnels
        // inputs: input_buffers, rootk input buffs for each funnel
        // output: middle_buffers, one for each funnel
        this->setup_bottom_funnels();

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
