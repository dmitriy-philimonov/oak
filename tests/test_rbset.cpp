#include "rbset.h"
#include <gtest/gtest.h>
#include <vector>

using namespace NRBTree;
using TKey = int;

#define V(...) std::vector<TKey>(__VA_ARGS__)


TEST(TSet, Basics) {
    TSet<TKey> set;
    set.insert({13,17,5,8,3,6,7});
    set.erase(5);
    
    std::vector<TKey> keys;
    set.PreOrder(keys);
    EXPECT_EQ(keys, V({13, 6, 3, 7, 8, 17}));

    keys.clear();
    set.InOrder(keys);
    EXPECT_EQ(keys, V({3,6,7,8,13,17}));

    keys.clear();
    set.PostOrder(keys);
    EXPECT_EQ(keys, V({3,8,7,6,17,13}));

    keys.clear();
    set.IncreaseOrder(keys);
    EXPECT_EQ(keys, V({3,6,7,8,13,17}));

    keys.clear();
    set.DecreaseOrder(keys);
    EXPECT_EQ(keys, V({17,13,8,7,6,3}));

    EXPECT_TRUE(set.exists(8));
    EXPECT_FALSE(set.exists(5));
    EXPECT_EQ(*set.select(4), 8);
    EXPECT_EQ(set.find(8).rank(), 4U);
}

TEST(TSet, Clear) {
    struct TKeyWrapper {
        std::vector<TKey>& Keys;
        TKey Key;
        bool operator <(const TKeyWrapper& other) const noexcept {
            return Key < other.Key;
        }
        ~TKeyWrapper() {
            Keys.push_back(Key);
        }
    };
    std::vector<TKey> keys;
    
    TSet<TKeyWrapper> set;
    set.insert({keys, 13});
    set.insert({keys, 17});
    set.insert({keys, 5});
    set.insert({keys, 8});
    set.insert({keys, 3});
    set.insert({keys, 6});
    set.insert({keys, 7});
    set.erase({keys, 5});
    
    keys.clear(); // clear destructors which are already called
    set.clear();
    
    EXPECT_EQ(keys, V({3,8,7,6,17,13}));
}

TEST(TSet, Delete) {
    TSet<TKey> set;
    set.insert({13,17,19,5,3,8,6,7});
    set.erase(19);

    EXPECT_EQ(set.find(13).color(), TTree::RED);
    EXPECT_EQ(set.find(17).color(), TTree::BLACK);
    
    set.erase(17);
    EXPECT_EQ(set.find(13).color(), TTree::BLACK);
    
    set.erase(13);
    std::vector<TKey> keys;
    set.PreOrder(keys);
    EXPECT_EQ(keys, V({5,3,7,6,8}));
}

TEST(TSet, Delete2) {
    TSet<TKey> set;
    set.insert({13,17,19,5,3,8,6,7});
    set.erase(8);
    set.erase(19);
    set.erase(13);
    
    std::vector<TKey> keys;
    set.PreOrder(keys);
    EXPECT_EQ(keys, V({5,3,7,6,17}));
}

TEST(TSet, Size) {
    TSet<TKey> set;
    
    set.insert({1,5,7});
    EXPECT_EQ(set.size(), 3U);
    
    set.erase(5);
    EXPECT_EQ(set.size(), 2U);
    
    set.erase(7);
    EXPECT_EQ(set.size(), 1U);
    
    set.erase(1);
    EXPECT_EQ(set.size(), 0U);
}

TEST(TMap, Iterators) {
    TMap<TKey,TKey> map;
    map.insert(19, 49);
    map.insert(13, 42);
    map.insert(17, 47);
    
    std::vector<TKey> keys;
    for(auto el: map) {
        keys.push_back(el.Key);
        keys.push_back(el.Data);
    }
    EXPECT_EQ(keys, V({13,42,17,47,19,49}));
    
    keys.clear();
    for(auto rit=map.rbegin(); rit!=map.rend(); ++rit) {
        keys.push_back(rit->Key);
        keys.push_back(rit->Data);
    }
    EXPECT_EQ(keys, V({19,49,17,47,13,42}));
}

TEST(TMap, SelectRank) {
    TMap<TKey, TKey> map;
    map.insert(19, 49);
    map.insert(13, 42);
    map.insert(17, 47);

    auto it = map.select(2);
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->Key, 17);
    EXPECT_EQ(it->Data, 47);
    EXPECT_EQ(it.rank(), 2U);

    auto prevIt = it; prevIt--;
    EXPECT_EQ(prevIt->Key, 13);
    EXPECT_EQ(prevIt.rank(), 1U);
    auto nextIt = it; nextIt++;
    EXPECT_EQ(nextIt->Key, 19);
    EXPECT_EQ(nextIt.rank(), 3U);
}
