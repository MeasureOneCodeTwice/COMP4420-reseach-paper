# Potential areas of comparison

## Data structures

| Datastructure | Cache oblivious optimized | word-RAM optimized |
| ------------- | ------------------------- | ------------------ |
| Binary Search tree | https://dl.acm.org/doi/10.5555/545381.545386 | AVL tree, red black tree, etc |
| File order matinence | https://erikdemaine.org/papers/BRICS2002/paper.pdf | https://en.wikipedia.org/wiki/Order-maintenance_problem |
| B-Tree | https://erikdemaine.org/papers/BRICS2002/paper.pdf | standard implementation, 1 page node size (SQLite) |
| Priority queues | https://erikdemaine.org/papers/BRICS2002/paper.pdf | standard implementation |
| Linked lists | https://erikdemaine.org/papers/BRICS2002/paper.pdf | standard implementation |

## Algorithms


| Algorithm     | Cache oblivious optimized | word-RAM optimized |
| ------------- | ------------------------- | ------------------ |
| Sorting | (Funnel sort) https://erikdemaine.org/papers/BRICS2002/paper.pdf | Java's multithreaded quicksort |
| Matrix mult | https://erikdemaine.org/papers/BRICS2002/paper.pdf | Strassen's algorithm |
