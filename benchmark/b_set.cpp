#include "rbset.h"
#include "vanemdeboas.h"

#include <set>
#include <unordered_set>
#include <benchmark/benchmark.h>

constexpr ui32 N = 10000;

// there was tests for insert - but, allegedly, it's the allocator's wars

static void NUMBERS_CUSTOM_SET_SEARCH(benchmark::State& state) {
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
static void NUMBERS_CUSTOM_SET_SELECT(benchmark::State& state) {
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
static void NUMBERS_STL_SET_SEARCH(benchmark::State& state) {
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
static void NUMBERS_STL_HASH_SEARCH(benchmark::State& state) {
    std::unordered_set<ui32> set;
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
static void NUMBERS_VANEMDEBOAS_SEARCH(benchmark::State& state) {
    NvanEmdeBoas::TTree<N-1> tree;
    for(ui32 i=0; i<N; ++i)
        tree.Insert(i);
    //timing
    for(auto _ : state) {
        ui32 i=N;
        while(i--)
            if (!tree.Member(i))
                std::cout << "BROKEN ON " << i << '\n';
    }
    state.SetLabel("Size="+std::to_string(tree.SizeU()));
}

// Register the functions as a benchmarks
BENCHMARK(NUMBERS_STL_SET_SEARCH);
BENCHMARK(NUMBERS_STL_HASH_SEARCH);
BENCHMARK(NUMBERS_CUSTOM_SET_SEARCH);
BENCHMARK(NUMBERS_CUSTOM_SET_SELECT);
BENCHMARK(NUMBERS_VANEMDEBOAS_SEARCH);