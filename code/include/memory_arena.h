#pragma once

#include <assert.h>
#include <new>
#include <span>

//have to have the implementation of templates in the definition. I wish it wasn't this way.
template <typename T>
class MemoryArena {
    public: 
        MemoryArena(int buff_size) {
            assert(buff_size >= 0);
            this->buffer_size = buff_size;
            this->buffer = new T[this->buffer_size];
        };

        std::span<T> allocate(int items) {
            const int space_left = this->buffer_size - this->items_allocated;
            if(items > space_left) {
                throw std::bad_alloc();
            }

            const std::span<T> span(this->buffer + items_allocated, items);
            this->items_allocated += items;
            return span;
        };

        ~MemoryArena() {
            delete[] buffer;
        };

    private:
        T* buffer = nullptr;
        int buffer_size;
        int items_allocated = 0; 
};
