#include "prefixdfamem.h"
#include <gtest/gtest.h>

using namespace NPrefix::NMemoryOptimized;

TEST(TPrefixDfa, Endings) {
    TDfa dfa;
    dfa.insert("she");
    dfa.insert("sea");
    dfa.insert("shell");

    EXPECT_TRUE(dfa.exists("she"));
    EXPECT_TRUE(dfa.exists("sea"));
    EXPECT_TRUE(dfa.exists("shell"));
    EXPECT_FALSE(dfa.exists("he"));
}

TEST(TPrefixDfa, EasyInsert) {
    TDfa dfa;
    dfa.insert("the");
    dfa.insert("this");

    EXPECT_TRUE(dfa.exists("the"));
    EXPECT_TRUE(dfa.exists("this"));
    EXPECT_FALSE(dfa.exists("tea"));
    EXPECT_FALSE(dfa.exists("where"));
}

TEST(TPrefixDfa, RealWordInsert) {
    TDfa dfa;
    dfa.insert("she");
    dfa.insert("by");
    dfa.insert("sea");
    dfa.insert("shells");
    dfa.insert("the");
    dfa.insert("sells");
    dfa.insert("shore");
    dfa.insert("sea");

    EXPECT_TRUE(dfa.exists("she"));
    EXPECT_TRUE(dfa.exists("sells"));
    EXPECT_TRUE(dfa.exists("sea"));
    EXPECT_TRUE(dfa.exists("shells"));
    EXPECT_TRUE(dfa.exists("by"));
    EXPECT_TRUE(dfa.exists("the"));
    EXPECT_TRUE(dfa.exists("shore"));
}

TEST(TPrefixDfa, CorrectUnfolding) {
    TDfa dfa;
    dfa.insert("shells");
    dfa.insert("she");
    EXPECT_TRUE(dfa.exists("she"));
}

TEST(TPrefixDfa, FitIntoWithEndAfterLastCharacter) {
    TDfa dfa;
    dfa.insert("she");
    dfa.insert("s");
    EXPECT_TRUE(dfa.exists("s"));
}