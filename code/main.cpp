#include <cstdlib>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <iostream>
#include <assert.h>
// #include <iomanip>
// #include <iostream>

class Buffer
{
    private:
        int capacity; // num of ints this buff can hold
        int read_offset;
        int write_offset;

        int* data;

    public:
        // pass in pointer to memory
        // the memory is managed in the funnels, the buffer just adds a layer of abstraction so we use it with methods
        Buffer(int capacity, int* data) : Buffer(capacity, 0, data){}

        Buffer(int capacity, int size, int* data)
        {
            this->capacity = capacity;
            this->read_offset = 0;
            this->write_offset = size;
            this->data = data;
        }

        // insert data to back of data array
        bool push_back(int val)
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
        int* peek_front()
        {
            int* val = nullptr;
            if (!this->is_empty())
            {
                val = &(this->data[this->read_offset]);
            }
            return val;
        }

        // read value from front of array
        int* read_front()
        {
            int* val = nullptr;
            if (!this->is_empty())
            {
                val = &(this->data[this->read_offset]);
                this->read_offset++;
            }
            return val;
        }

        int size()
        {
            return this->write_offset - this->read_offset;
        }

        bool is_empty()
        {
            return this->size() <= 0;
        }

        bool is_full()
        {
            return this->size() >= this->capacity;
        }
};


class KFunnel
{
    private:
        const int BASE_K = 2;

        int kcubed;
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

    public:
        KFunnel(int k, std::vector<Buffer*> input_buffers, Buffer* output_buffer)
        {
            this->k = k;
            this->rootk = std::ceil( std::sqrt(k) );
            this->kcubed = std::pow(k, 3);

            this->input_buffers = input_buffers;
            this->output_buffer = output_buffer;

            this->middle_buffers_memory = nullptr; // base case, make it null

            // recursively create funnels until we hit the base case
            // Base Case Funnel (assuming BASE_K = 2):
            // - 2 input buffers, and 1 output buffer
            // Recursive Case Funnel:
            // - rootk-funnels nested inside, 
            //   with middle_buffers connecting top and bottom funnels
            if (k > BASE_K)
            {
                // create middle buffers
                int middle_buffer_size = std::pow(this->rootk, 3);
                int middle_buffers_total_space = middle_buffer_size * rootk;
                this->middle_buffers_memory = new int[middle_buffers_total_space];
                for(int i = 0; i < rootk; i++)
                {
                    int offset = i * middle_buffer_size;
                    this->middle_buffers.push_back(new Buffer(middle_buffer_size, &this->middle_buffers_memory[offset]));
                }
    
                // create top funnel
                // inputs: middle_buffers
                // output: output_buffer
                this->top_funnel = new KFunnel(rootk, this->middle_buffers, this->output_buffer);
    
                // create bottom funnels
                // inputs: input_buffers, rootk input buffs for each funnel
                // output: middle_buffers, one for each funnel
                for(int i = 0; i < rootk; i++)
                {
                    // may not have a perfect number of inputs buffers for each bottom funnel, 
                    // last funnel may have fewer inputs
                    int start_index = i * rootk;
                    int end_index = std::min( (int)this->input_buffers.size(), (i + 1) * rootk);
                    std::vector<Buffer*> sliced_input_buffers;
                    if (end_index > start_index)
                    {
                        auto start = this->input_buffers.begin() + start_index;
                        auto end = this->input_buffers.begin() + end_index;
                        sliced_input_buffers.assign(start, end);
                    }
                    
                    bottom_funnels.push_back(new KFunnel(rootk, sliced_input_buffers, this->middle_buffers[i]));
                }
            }
        }

        bool fill_output_buffer()
        {
            bool is_filled = this->output_buffer->is_full();
            if (!is_filled)
            {
                // need to fill the output buffer
                // -> grab data inputs of nested top_funnel

                // Recursive Case Fill:
                // - nested funnel, so pass the operation to it
                // Base Case Fill:
                // - no nested funnels, grab min from input and float it up
                if (this->k <= BASE_K)
                {
                    // Base Case
                    // no middle_buffers, just grab from input buffers
                    // until the output is filled or inputs are drained
                    bool input_drained = false;
                    while(!this->output_buffer->is_full() && !input_drained)
                    {
                        int* min_val = this->retrieve_min_from_input_buffers();
                        input_drained = min_val == nullptr;
                        if (!input_drained)
                        {
                            this->output_buffer->push_back(*min_val);
                        }
                    }
                    is_filled = this->output_buffer->is_full();
                }
                else
                {
                    // recursive case
                    // call fill on the nested top_funnel
                    is_filled = this->top_funnel->fill_output_buffer();
                }
            }
            return is_filled;
        }

        int* retrieve_min_from_input_buffers()
        {
            int* min_input_val = nullptr;
            int buffer_index = -1;
            for(int i = 0; i < this->input_buffers.size(); i++)
            {
                if (this->input_buffers[i]->is_empty())
                {
                    // must fill the input buffer first
                    // -> relies on parent funnel to do so
                    // TODO: ....NEED TO FILL THIS IN...
                    // somehow need to inform the parent which
                    //   funnel needs to be activated
                    //   maybe return a STATUS enum of sorts?
                    //   need to also account for the eventual base case
                    //   of actually emptying all the inputs (sorting complete)
                    //    maybe need to stop using the inputs that are dry
                }

                // the input buffer should have been filled by now
                // EXCEPT: in the case where this input buffer and its downstream inputs are all drained
                if (!this->input_buffers[i]->is_empty())
                {
                    int* curr_val = this->input_buffers[i]->peek_front();
    
                    assert(curr_val != nullptr); // should not be an empty buffer
                    
                    // init min_input_val if not yet assigned
                    if (min_input_val == nullptr)
                    {
                        min_input_val = curr_val;
                        buffer_index = i;
                    }
                    else if (*curr_val < *min_input_val)
                    {
                        // assign the new min
                        min_input_val = curr_val;
                        buffer_index = i;
                    }
                }
            }
            
            if (min_input_val != nullptr)
            {
                // we found a min value from the input buffers
                // actually read it now, moving the read_offset forward on that input buffer
                min_input_val = this->input_buffers[buffer_index]->read_front();
            }
            else
            {
                // all downstream inputs for this funnel must be empty
                // need to stop using this funnel
                // or just waste some instructions every fill stage...
            }
            return min_input_val;
        }
};

int main(int argc, char *argv[])
{

    const int n = 8; 
    int input[] = {5,8,3,2,7,1,9,4};
    int output[n];
    int k = std::cbrt(n);

    Buffer* output_buffer = new Buffer(n, output);

    std::vector<Buffer*> input_buffers;
    int input_sizes = n / k;
    for(int i = 0; i < k; i++)
    {
        int offset = i * input_sizes;
        offset = std::min(offset, n - 1); // in case n is not a nice number
        
        int buffer_size = std::min(input_sizes, n - offset);

        if (buffer_size > 0)
        {
            input_buffers.push_back(new Buffer(buffer_size, buffer_size, &input[offset]));
        }
    }
    
    KFunnel* k2_funnel = new KFunnel(k, input_buffers, output_buffer);

    k2_funnel->fill_output_buffer();
    
    return EXIT_SUCCESS;
}



// int compute_local_buffer_space(int sqrtK)
// {
//     // space needed for only this funnel
//     // using sqrtK

//     int bufferSize = std::ceil( std::pow(sqrtK, 3) );
//     int numBuffers = sqrtK;
//     int totalBufferSpace = bufferSize * numBuffers;

//     return totalBufferSpace;
// }

// int compute_total_buffer_space(int k)
// {
//     int sqrtK = std::ceil( std::sqrt(k) );
//     int totalSpace = 0;

//     if(sqrtK <= 2){
//         // base case
//         totalSpace = compute_local_buffer_space(sqrtK);

//         std::cout << "--REACHED BASE CASE--\n";
//         std::cout << "\t" << sqrtK << "-funnel needs " << totalSpace << " space\n";
//     }else{
//         // recursive case

//         // buffers for this funnel
//         int thisBuffers = compute_local_buffer_space(sqrtK);
        
//         // space needed for each of the subfunnels
//         int spacePerSubfunnel = compute_total_buffer_space(sqrtK);
        
//         // Total Space:
//         // buffers for all rootK bottom subfunnels + one top subfunnel + buffers for this funnel
//         totalSpace += (sqrtK + 1) * spacePerSubfunnel;
//         totalSpace += thisBuffers;
        
//         std::cout << "--RECURSIVE CASE--\n";
        
//         std::cout << "\tThis Funnel's Buffer Space: " << thisBuffers << " space\n";

//         std::cout << "\tTOTAL SPACE: " << sqrtK << " * " << sqrtK << "-funnels need " << totalSpace << " space\n";

//     }

//     return totalSpace;
// }