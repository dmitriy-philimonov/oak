#include "vanemdeboas.h"
#include <gtest/gtest.h>
#include <vector>


using namespace NvanEmdeBoas;
#define V(...) std::vector<ui32>(__VA_ARGS__)


TEST(vanEmdeBoas, Delete) {
    TTree<15> tree;
    tree.InsertRange({2,3,4,5,7,14,15});
    tree.Delete(14);
    EXPECT_EQ(tree.Successor(7U), 15U);
}
TEST(vanEmdeBoas, DoubleInsert) {
    TTree<15> tree;
    EXPECT_TRUE (tree.Insert(2));
    EXPECT_FALSE(tree.Insert(2));
    EXPECT_TRUE (tree.Insert(3));
    EXPECT_FALSE(tree.Insert(3));
    EXPECT_TRUE (tree.Insert(5));
    EXPECT_FALSE(tree.Insert(5));
    EXPECT_TRUE (tree.Insert(6));
    EXPECT_FALSE(tree.Insert(6));
    EXPECT_TRUE (tree.Insert(7));
    EXPECT_FALSE(tree.Insert(7));
}
TEST(vanEmdeBoas, DoubleDelete) {
    TTree<15> tree;
    EXPECT_TRUE (tree.Insert(2));
    EXPECT_TRUE (tree.Delete(2));
    EXPECT_FALSE(tree.Delete(2));
}
TEST(vanEmdeBoas, DoubleDelete2) {
    TTree<15> tree;
    EXPECT_TRUE (tree.Insert(2));
    EXPECT_TRUE (tree.Insert(3));
    EXPECT_TRUE (tree.Delete(3));
    EXPECT_FALSE(tree.Delete(3));
}
TEST(vanEmdeBoas, DoubleDelete3) {
    TTree<15> tree;
    EXPECT_TRUE (tree.Insert(4));
    EXPECT_TRUE (tree.Insert(5));
    EXPECT_TRUE (tree.Insert(6));
    EXPECT_TRUE (tree.Insert(7));
    EXPECT_FALSE(tree.Delete(15));
    EXPECT_TRUE (tree.Delete(7));
    EXPECT_FALSE(tree.Delete(7));
    EXPECT_TRUE (tree.Delete(4));
    EXPECT_FALSE(tree.Delete(4));
    EXPECT_TRUE (tree.Delete(6));
    EXPECT_FALSE(tree.Delete(6));
}
TEST(vanEmdeBoas, SuccessorAndPredecessor) {
    TTree<15> tree; std::vector<ui32> v; v.reserve(16);
    tree.InsertRange({2,3,4,5,7,14,15});

    for(ui32 i = tree.Minimum(); i != NIL; i = tree.Successor(i))
        v.push_back(i);
    EXPECT_EQ(v, V({2,3,4,5,7,14,15}));

    v.clear();
    for(ui32 i = tree.Maximum(); i != NIL; i = tree.Predecessor(i))
        v.push_back(i);
    EXPECT_EQ(v, V({15,14,7,5,4,3,2}));
}