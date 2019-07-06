#include "pascal_triangle.h"

#include <gtest/gtest.h>

constexpr ui32 N=7;

TEST(PASCAL, SIMPLE) {
    TPyramid<N> p; p.TestFill();

    EXPECT_EQ(p(N,  0), N);
    EXPECT_EQ(p(N,N/2), N);
    EXPECT_EQ(p(N,N-1), N);
}

TEST(PASCAL, MATRIX_ADAPTER) {
    TMatrixRightTopAdaptor<N> M; M.TestFill();
    ui32 i=0; ui32 j=0; ui32 n = N;
    while(n) {
        i = 0;
        j = N - n;
        do {
            EXPECT_EQ(M(i,j), n);
            ++i; ++j;
        } while(j<N);
        --n;
    }
}

TEST(PASCAL, TRIANGLE) {
    /* compile time test */
    constexpr TPascalTriangle<N> P;
    static_assert(P.C(6,3) == 20U);
}