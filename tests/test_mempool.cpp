#include "mempool.h"
#include <gtest/gtest.h>

using namespace NMemory;


TEST(TPool, NearestPowerOfTwo) {
    EXPECT_EQ(TUtil::NearestPowerOfTwo(15), 16ULL);
    EXPECT_EQ(TUtil::NearestPowerOfTwo(16), 16ULL);

    EXPECT_EQ(TUtil::NearestPowerOfTwo(0), 0ULL);
    EXPECT_EQ(TUtil::NearestPowerOfTwo(1), 1ULL);
    EXPECT_EQ(TUtil::NearestPowerOfTwo(3), 4ULL);

    EXPECT_EQ(TUtil::NearestPowerOfTwo(61000), 65536ULL);
}

TEST(TPool, Linear) {
    struct T {
        ui32 x = 11;
        ui32 y = 13;
        ui32 z = 17;
    };
    TPool<TLinearStrategy> pool(16);
    T* p1 = pool.Append(T()); // 1(12/16)
    EXPECT_EQ(pool.BlocksCount(), 1ULL);
    T* p2 = pool.Append(T()); // 1(12/16) 2(12/16)
    EXPECT_EQ(pool.BlocksCount(), 2ULL);
    T* p3 = pool.Append(T()); // 1(12/16) 2(12/16) 3(12/16)
    EXPECT_EQ(pool.BlocksCount(), 3ULL);

    p1->~T(); p2->~T(); p3->~T();
}

TEST(TPool, Exponential) {
    struct T {
        ui32 x = 11;
        ui32 y = 13;
        ui32 z = 17;
    };
    TPool<TExponentialStrategy> pool(16);
    pool.Append(T()); // 1(12/16)
    EXPECT_EQ(pool.BlocksCount(), 1ULL);

    pool.Append(T()); // 1(12/16) 2(12/32)
    pool.Append(T()); // 1(12/16) 2(24/32)
    EXPECT_EQ(pool.BlocksCount(), 2ULL);

    pool.Append(T()); // 1(12/16) 2(24/32) 3(12/64)
    pool.Append(T()); // 1(12/16) 2(24/32) 3(24/64)
    pool.Append(T()); // 1(12/16) 2(24/32) 3(36/64)
    pool.Append(T()); // 1(12/16) 2(24/32) 3(48/64)
    pool.Append(T()); // 1(12/16) 2(24/32) 3(60/64)
    EXPECT_EQ(pool.BlocksCount(), 3ULL);
}

TEST(TPool, String) {
    TPool<> pool(20); // real block size 2**(ceil(log2(20))) = 32
    char* s = pool.Append("Hello"); // 0+6=6
    pool.Append("world");           // 6+6=12
    pool.Append("Ура!");            // 12+8=20 (utf8)

    EXPECT_STREQ(s, "Hello");
    s += 6;
    EXPECT_STREQ(s, "world");
    s += 6;
    EXPECT_STREQ(s, "Ура!");
    s += 8;
    std::memset(s, '\0', 12); // clear next 12 bytes

    char* p = pool.Append("Я рад за Вас"); // size=22, 20+22 > 32 -> new block
    EXPECT_EQ(*s, '\0');                   // first block wasn't affected
    EXPECT_EQ(pool.BlocksCount(), 2ULL);   // we have 2 blocks
    EXPECT_STREQ(p, "Я рад за Вас");       // yeah, our string in the second block
}