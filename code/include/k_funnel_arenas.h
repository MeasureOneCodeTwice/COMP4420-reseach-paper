#include "memory_arena.h"
#include "buffer.h"

struct KFunnelArenas {
    //need to use a pointer so members aren't freed when the struct is.
    MemoryArena<int> *buffer_space_arena;
    MemoryArena<Buffer> *buffer_arena;
    MemoryArena<KFunnel> *funnel_arena;
    ~KFunnelArenas () {} 
};
