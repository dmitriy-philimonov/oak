#include "prefixtree.h"
#include <gtest/gtest.h>

using namespace NPrefix;

#define V(...) NPrefix::TKeyRefs({__VA_ARGS__})

template<size_t N>
std::string_view E(const char(&cstr)[N]){
    // s[N-1] is '\0', what is so desired
    return std::string_view(&cstr[0], N);
}
std::string_view E(){
    static char null[] = ""; // implicit '\0' in the end
    return std::string_view(&null[0], 1);
}

TEST(TPrefixTree, AppendRepeat) {
    TTree tree;
    tree.Append("aba");
    tree.Append("abab");
    tree.Append("aba");

    EXPECT_EQ(tree.InOrder(), V("aba", E(), E("b")));
}
TEST(TPrefixTree, AppendComplex) {
    TTree tree;
    tree.Append("bc");
    tree.Append("baca");
    tree.Append("bac");
    tree.Append("abab");
    tree.Append("b");
    tree.Append("bc");
    tree.Append("aba");

    EXPECT_TRUE(tree.Exists("baca"));
    EXPECT_EQ(tree.InOrder(), V("aba", E(), E("b"), "b", E(), "ac", E(), E("a"), E("c")));
}
TEST(TPrefixTree, AppendRealWords) {
    TTree tree;
    tree.Append("she");
    tree.Append("sells");
    tree.Append("sea");
    tree.Append("shells");
    tree.Append("by");
    tree.Append("the");
    tree.Append("sea");
    tree.Append("shore");
    EXPECT_EQ(tree.InOrder(), V(E("by"), "s", "e", E("a"), E("lls"), "h", "e", E(), E("lls"), E("ore"), E("the")));
}
TEST(TPrefixTree, Remove) {
    TTree tree;
    tree.Append("bac");
    tree.Append("baca");
    tree.Remove("bac");
    EXPECT_EQ(tree.size(), 1U);
    EXPECT_TRUE(tree.Exists("baca"));
    EXPECT_FALSE(tree.Exists("bac"));

    tree.Remove("baca");
    EXPECT_EQ(tree.size(), 0U);
}
TEST(TPrefixTree, RemoveRealWords) {
    TTree tree;
    tree.Append("she");
    tree.Append("sells");
    tree.Append("sea");
    tree.Append("shells");
    tree.Append("by");
    tree.Append("the");
    tree.Append("sea");
    tree.Append("shore");

    tree.Remove("sea");
    tree.Remove("shells");
    EXPECT_FALSE(tree.Exists("sea"));
    EXPECT_FALSE(tree.Exists("shells"));
}
