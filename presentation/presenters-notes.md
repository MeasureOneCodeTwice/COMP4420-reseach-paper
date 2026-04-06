# Total Time: 6.5 min

We are Logan and Juan. During our project, we tried to find an algorithm that would optimize cache misses.

# 1. Why?
> 20 sec
> **00:20**

Why would one care about cache misses on modern hardware? This is the hardware on my home PC. Fetching a single word from RAM for the first time takes around 10 ns. In this amount of time, my CPU has already ran 54 clock cycles. So my home PC may care more about optimizing memory fetches than it does instruction count.

# 2. Assumptions
> 20 sec
> **0:40**

So far in our algorithms courses,we've always examined algorithms within the Word RAM model, where memory transfers are free while instructions cost. We will need to switch our brains to the Cache-Oblivious model for our analysis, where instructions are free and memory transfers are costly. That is, so far we've always optimized for number of instructions, but we now want to optimize for the number of memory transfers.

# 3. Refresher: Cache, Words, and Blocks
> 20 sec
> **1:00**

First, a quick refresher on the inner workings. Within our hierarchy of cache levels, whenever we need just a single word from the lower level, say from Memory, we need to fetch the entire block into cache. And this means we also have to evict something from our cache to make space. Since we only needed $1$ word, all $B-1$ words from this block are wasted in cache. 

# 4. Building Intuition
> 30 sec
> **1:30**

This cache optimization analysis is hard to intuit when staring at code, so let's stare at some code. 

On the left, we simply increment a counter. This is Theta of $n$ runtime complexity, while completely free in cache complexity.

On the right, we scan through an array. This is Oh of $n$ runtime complexity, while Oh of $N/B$ cache complexity. 

Why $N/B$? Cap $N$ is the number of words we access in the cache-oblivious model, kind of how lowercase $n$ is the instruction count in the Word RAM model. $B$ is the number of words in a block. The entire array will occupy $N/B$ blocks, which is the number of memory fetches required.

# 5. Cache-Oblivious Model: Formal Definition
> 30 sec
> **2:00**

You're ready for the formal definition. We already saw a little about $N$ and $B$. $M$ is the total number of words in our higher level cache. This means our cache has $M/B$ cache lines, or block capacity.

Since we're 

# 6. (1) Recall Mergesort
> 45 sec
> **2:45**

# 6. (2) Recall Mergesort
> 20 sec
> **3:05**

# 7. Lazy Funnelsort: Introduction
> 45 sec
> **3:50**

# 8. Lazy Funnelsort: Single K-Funnel Instance
> 45 sec
> **4:35**

# 9. Lazy Funnelsort: Base Case
> 30 sec
> **5:05**

# 10 References

