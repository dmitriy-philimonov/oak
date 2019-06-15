#include "prefixdfa.h"
#include <gtest/gtest.h>

using namespace NPrefix;

TEST(TPrefixDfa, EasyInsert) {
    TDfa dfa;
    dfa.insert("she");
    dfa.insert("sea");
    dfa.insert("shell");

    EXPECT_TRUE(dfa.exists("she"));
    EXPECT_TRUE(dfa.exists("sea"));
    EXPECT_TRUE(dfa.exists("shell"));
    EXPECT_FALSE(dfa.exists("he"));
}

TEST(TPrefixDfa, EasyInsert2) {
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

TEST(TPrefixDfa, ShortWordAfterLongWord) {
    TDfa dfa;
    dfa.insert("she");
    dfa.insert("s");
    EXPECT_TRUE(dfa.exists("s"));
}

TEST(TPrefixDfa, Erase) {
    TDfa dfa;
    dfa.insert("she");
    dfa.insert("sea");
    dfa.insert("shell");
    dfa.insert("shells");

    dfa.erase("she");
    EXPECT_TRUE(dfa.exists("shell"));

    dfa.erase("shell");
    EXPECT_TRUE(dfa.exists("sea"));
    EXPECT_TRUE(dfa.exists("shells"));

    dfa.erase("shells");
    EXPECT_TRUE(dfa.exists("sea"));

    dfa.insert("seasalt");
    dfa.insert("sun");
    EXPECT_TRUE(dfa.exists("sea"));
    EXPECT_TRUE(dfa.exists("seasalt"));
    EXPECT_TRUE(dfa.exists("sun"));

    dfa.erase("sea");
    EXPECT_TRUE(dfa.exists("seasalt"));

    dfa.erase("seasalt");
    EXPECT_TRUE(dfa.exists("sun"));

    dfa.erase("sun");
    EXPECT_FALSE(dfa.exists("sun"));
    EXPECT_FALSE(dfa.erase("sea"));
}

TEST(TPrefixDfa, EraseAll) {
    TDfa dfa;
    dfa.insert("the");
    dfa.insert("this");

    dfa.erase("this");
    dfa.erase("the");

    dfa.insert("these");
    EXPECT_TRUE(dfa.exists("these"));
}