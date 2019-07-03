#include "disjoint_forest.h"

#include <gtest/gtest.h>
using namespace NDisjointSetForest;

TEST(DisjointForest, Simple) {
    TTree tree;
    tree.MakeSet(1);
    tree.MakeSet(2);
    tree.MakeSet(3);
    tree.MakeSet(4);
    tree.MakeSet(5);
    tree.MakeSet(6);

    tree.Union(1,2);
    tree.Union(2,6);
    tree.Union(1,4);

    EXPECT_EQ(tree.FindSet(1), tree.FindSet(2));
    EXPECT_EQ(tree.FindSet(1), tree.FindSet(4));
    EXPECT_EQ(tree.FindSet(1), tree.FindSet(6));
    EXPECT_EQ(tree.FindSet(2), tree.FindSet(4));
    EXPECT_EQ(tree.FindSet(2), tree.FindSet(6));
    EXPECT_EQ(tree.FindSet(4), tree.FindSet(6));

    EXPECT_NE(tree.FindSet(1), tree.FindSet(3));
    EXPECT_NE(tree.FindSet(1), tree.FindSet(5));
}