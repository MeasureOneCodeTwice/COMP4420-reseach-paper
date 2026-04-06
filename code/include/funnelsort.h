#include <span>
#include<limits.h>

const int MAX_INPUT_BUFF_WO_RECURSION = 512;

void funnel_sort(std::span<int> input, bool printDebugInfo = false);
