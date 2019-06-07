#include "rbset.h"

#include <set>
#include <benchmark/benchmark.h>

using ui32 = uint32_t;
constexpr ui32 N = 10000;

// there was tests for insert - but, allegedly, it's the allocator's wars

static void RBTREE_CUSTOM_SET_SEARCH(benchmark::State& state) {
    //setup
    NRBTree::TSet<ui32> set;
    for(ui32 i=0; i<N; ++i)
        set.insert(i);
    //timing
    for(auto _ : state) {
        ui32 i=N;
        while(i--)
            if (set.find(i) == set.end())
                std::cout << "BROKEN ON " << i << '\n';
    }
    state.SetLabel("Size="+std::to_string(set.size()));
}
static void RBTREE_CUSTOM_SET_SELECT(benchmark::State& state) {
    NRBTree::TSet<ui32> set;
    for(ui32 i=0; i<N; ++i)
        set.insert(i);
    for(auto _ : state) {
        /* remember 1-based */
        for(ui32 i=N; i!=0; --i)
            if (set.select(i) == set.end())
                std::cout << "BROKEN ON " << i << '\n';
    }
    state.SetLabel("Size="+std::to_string(set.size()));
}
static void RBTREE_STL_SET_SEARCH(benchmark::State& state) {
    std::set<ui32> set;
    for(ui32 i=0; i<N; ++i)
        set.insert(i);
    for(auto _ : state) {
        ui32 i=N;
        while(i--)
            if (set.find(i) == set.end())
                std::cout << "BROKEN ON " << i << '\n';
    }
    state.SetLabel("Size="+std::to_string(set.size()));
}

// Register the functions as a benchmarks
BENCHMARK(RBTREE_STL_SET_SEARCH);
BENCHMARK(RBTREE_CUSTOM_SET_SEARCH);
BENCHMARK(RBTREE_CUSTOM_SET_SELECT);