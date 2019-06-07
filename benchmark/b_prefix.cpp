#include "prefixdfa.h"
#include "prefixdfamem.h"
#include "prefixtree.h"
#include "rbset.h"
#include "benchmark/benchmark.h"

#include <iostream>
#include <fstream>
#include <deque>
#include <set>
#include <unordered_set>

class TWarAndPiece{
private:
    std::deque<std::string> Words;
private:
    std::string Filter(const std::string& word) {
        static const std::string filter_out(",.“");
        std::string r;
        for (size_t i=0; i<word.size(); ++i) {
            if (filter_out.find(word[i]) == std::string::npos)
                r += static_cast<char>(tolower(word[i]));
        }
        return r;
    }
    void ReadFile(const std::string& fileName) {
        std::ifstream file(fileName); std::string word;
        while(file >> word) {
            if (std::none_of(word.begin(), word.end(), [](char c) { return std::isalpha(c);}))
                continue;
            Words.push_back(Filter(word));
        }
    }
public:
    TWarAndPiece() {
        ReadFile("../../war_and_peace.txt");
    }
    size_t size() const noexcept { return Words.size(); }

    using TIt = std::deque<std::string>::const_iterator;
    TIt begin() const noexcept { return Words.begin(); }
    TIt end() const noexcept { return Words.end(); }
} wap;

struct TTest {
    NPrefix::TDfa Dfa;
    NPrefix::NMemoryOptimized::TDfa DfaMO;
    NPrefix::TTree PrefixTree;
    NRBTree::TSet<std::string> RBTree;
    std::set<std::string> StlSet;
    std::unordered_set<std::string> StlUOSet;

    TTest() {
        for(const auto& word: wap) {
            Dfa.insert(word);
            DfaMO.insert(word);
            PrefixTree.Append(word);
            RBTree.insert(word);
            StlSet.insert(word);
            StlUOSet.insert(word);
        }
    }
} t;

/* SEARCH KEY TEST */

static void SMALL_STLUNORDEREDSET_SEARCH_LONG_WORD(benchmark::State& state) {
    const auto& hash = t.StlUOSet;
    for(auto _ : state)
        if (hash.find("http://wwwgutenbergorg/2/6/0/2600/") == hash.end())
            std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void SMALL_STLUNORDEREDSET_SEARCH_SHORT_WORD(benchmark::State& state) {
    const auto& hash = t.StlUOSet;
    for(auto _ : state)
        if (hash.find("the") == hash.end())
            std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void SMALL_DFA_SEARCH_LONG_WORD(benchmark::State& state) {
    const auto& dfa = t.Dfa;
    for(auto _ : state)
        if (!dfa.exists("http://wwwgutenbergorg/2/6/0/2600/"))
            std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size())
                 + ",size=" + std::to_string(dfa.size())
                 + ",maxState="+std::to_string(dfa.StateCount())
    );
}
static void SMALL_DFA_SEARCH_SHORT_WORD(benchmark::State& state) {
    const auto& dfa = t.Dfa;
    for(auto _ : state)
        if (!dfa.exists("the"))
            std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size())
                 + ",size=" + std::to_string(dfa.size())
                 + ",maxState="+std::to_string(dfa.StateCount())
    );
}
static void SMALL_DFAMO_SEARCH_LONG_WORD(benchmark::State& state) {
    const auto& dfa = t.DfaMO;
    for(auto _ : state)
        if (!dfa.exists("http://wwwgutenbergorg/2/6/0/2600/"))
            std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size())
                 + ",size=" + std::to_string(dfa.size())
                 + ",maxState="+std::to_string(dfa.StateCount())
    );
}
static void SMALL_DFAMO_SEARCH_SHORT_WORD(benchmark::State& state) {
    const auto& dfa = t.DfaMO;
    for(auto _ : state)
        if (!dfa.exists("the"))
            std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size())
                 + ",size=" + std::to_string(dfa.size())
                 + ",maxState="+std::to_string(dfa.StateCount())
    );
}

BENCHMARK(SMALL_STLUNORDEREDSET_SEARCH_SHORT_WORD);
BENCHMARK(SMALL_STLUNORDEREDSET_SEARCH_LONG_WORD);
BENCHMARK(SMALL_DFA_SEARCH_SHORT_WORD);
BENCHMARK(SMALL_DFA_SEARCH_LONG_WORD);
BENCHMARK(SMALL_DFAMO_SEARCH_SHORT_WORD);
BENCHMARK(SMALL_DFAMO_SEARCH_LONG_WORD);

/* INSERT TESTS */

static void PREFIX_DFA_INSERT(benchmark::State& state) {
    NPrefix::TDfa dfa;
    for(auto _ : state) {
        for(const auto& word: wap)
            dfa.insert(word);
    }
    state.SetLabel("Words="+std::to_string(wap.size())+",size="+std::to_string(dfa.size()));
}
static void PREFIX_DFAMO_INSERT(benchmark::State& state) {
    NPrefix::NMemoryOptimized::TDfa dfa;
    for(auto _ : state) {
        for(const auto& word: wap)
            dfa.insert(word);
    }
    state.SetLabel("Words="+std::to_string(wap.size())+",size="+std::to_string(dfa.size()));
}
static void PREFIX_STLUNORDEREDSET_INSERT(benchmark::State& state) {
    std::unordered_set<std::string> hash;
    for(auto _ : state) {
        for(const auto& word: wap)
            hash.insert(word);
    }
    state.SetLabel("Words="+std::to_string(wap.size())+",size="+std::to_string(hash.size()));
}
static void PREFIX_PREFIXTREE_INSERT(benchmark::State& state) {
    NPrefix::TTree tree;
    for(auto _ : state) {
        for(const auto& word: wap)
            tree.Append(word);
    }
    state.SetLabel("Words="+std::to_string(wap.size())+",size="+std::to_string(tree.size()));
}
static void PREFIX_RBTREE_INSERT(benchmark::State& state) {
    NRBTree::TSet<std::string> set;
    for(auto _ : state) {
        for(const auto& word: wap)
            set.insert(word);
    }
    state.SetLabel("Words="+std::to_string(wap.size())+",size="+std::to_string(set.size()));
}
static void PREFIX_STLSET_INSERT(benchmark::State& state) {
    std::set<std::string> set;
    for(auto _ : state) {
        for(const auto& word: wap)
            set.insert(word);
    }
    state.SetLabel("Words="+std::to_string(wap.size())+",size="+std::to_string(set.size()));
}

BENCHMARK(PREFIX_DFA_INSERT);
BENCHMARK(PREFIX_DFAMO_INSERT);
BENCHMARK(PREFIX_STLUNORDEREDSET_INSERT);
BENCHMARK(PREFIX_PREFIXTREE_INSERT);
BENCHMARK(PREFIX_RBTREE_INSERT);
BENCHMARK(PREFIX_STLSET_INSERT);

/* AGGREGATE SEARCH TEST */

static void PREFIX_DFA_SEARCH(benchmark::State& state) {
    const auto& dfa = t.Dfa;
    for(auto _ : state)
        for(const auto& word: wap)
            if (!dfa.exists(word))
                std::cout << "BROKEN DFA ON WORD " << word << "\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void PREFIX_DFAMO_SEARCH(benchmark::State& state) {
    const auto& dfa = t.DfaMO;
    for(auto _ : state)
        for(const auto& word: wap)
            if (!dfa.exists(word))
                std::cout << "BROKEN DFA ON WORD " << word << "\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void PREFIX_STLUNORDEREDSET_SEARCH(benchmark::State& state) {
    const auto& hash = t.StlUOSet;
    for(auto _ : state)
        for(const auto& word: wap)
            if (hash.find(word) == hash.end())
                std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void PREFIX_STLSET_SEARCH(benchmark::State& state) {
    const auto& set = t.StlSet;
    for(auto _ : state)
        for(const auto& word: wap)
            if (set.find(word) == set.end())
                std::cout << "Ваууууу!\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void PREFIX_PREFIXTREE_SEARCH(benchmark::State& state) {
    const auto& tree = t.PrefixTree;
    for(auto _ : state)
        for(const auto& word: wap)
            if (!tree.Exists(word))
                std::cout << "BROKEN TREE ON WORD " << word << "\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}
static void PREFIX_RBTREE_SEARCH(benchmark::State& state) {
    const auto& set = t.RBTree;
    for(auto _ : state)
        for(const auto& word: wap)
            if (!set.exists(word))
                std::cout << "BROKEN TREE ON WORD " << word << "\n";
    state.SetLabel("Words=" + std::to_string(wap.size()));
}

BENCHMARK(PREFIX_DFA_SEARCH);
BENCHMARK(PREFIX_DFAMO_SEARCH);
BENCHMARK(PREFIX_STLUNORDEREDSET_SEARCH);
BENCHMARK(PREFIX_PREFIXTREE_SEARCH);
BENCHMARK(PREFIX_RBTREE_SEARCH);
BENCHMARK(PREFIX_STLSET_SEARCH);

/* 
 * Separate memory consumption benchmark shows:
 * // count(uniq(keys)) = 27185, count(keys) = 565622
 * 
 *   CONTAINER    VSS    VM
 * ==========================
 * unordered_set 4744K 15636K
 *    set        5008K 15784K
 * NRBTree::TSet 5076K 15792K
 * NPrefix::Tree 5332K 16064K
 * NPrefix::TDfa 9484K 21000K
 * NMemOpt::TDfa 6712K 18132K
 */