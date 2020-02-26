#pragma once


#include "defines.h"


#include <array>
#include <iostream>

/* l = [0, L]

   l = 1 - one element in a row
   l = 2 - two elements in a row
   l = 3 - three elements in a row
   l = 4 - four elements in a row
    ....

   example: in each level all values are equal to the level number

         1
        2 2
       3 3 3
      4 4 4 4
     5 5 5 5 5
    6 6 6 6 6 6

 */
template<ui32 L, typename I=ui32>
class TPyramid {
private:
    /* i = [1..N] */
    static constexpr ui32 Offset(ui32 i) noexcept {
        return ((i-1)*i) >> 1;
    }

    static constexpr ui32 SIZE = Offset(L+1);
    std::array<I, SIZE> S = {0};
public:
    /* l = [1..N], o = [0, i) */
    constexpr I& operator()(ui32 l, ui32 o) noexcept {
        return S[Offset(l)+o];
    }
    constexpr I operator()(ui32 l, ui32 o) const noexcept {
        return S[Offset(l)+o];
    }

    void TestFill() noexcept {
        for(ui32 l=1; l<=L; ++l)
            for(ui32 o=0; o<l; ++o)
                this->operator()(l,o) = l;
    }
    void DebugPrint() const noexcept {
        std::cout << "SIZE=" << sizeof(S) << "b {";
        for(ui32 i=0; i<SIZE; ++i)
            std::cout << S[i] << ',';
        std::cout << "\b}\n";
    }
};

/* Matrix of size N, right top part of which stored as array to save memory */
template<ui32 N>
class TMatrixRightTopAdaptor {
    TPyramid<N> S;
    /* restrictions: j < N, i<=j */
    static constexpr ui32 Level(ui32 i, ui32 j) noexcept {
        return N-j+i;
    }
public:
    /* restrictions: j < N, i<=j */
    constexpr ui32& operator()(ui32 i, ui32 j) noexcept {
        /* constexpr ui32 offset = i; */
        return S(Level(i,j), i);
    }
    constexpr ui32 operator()(ui32 i, ui32 j) const noexcept {
        return S(Level(i,j), i);
    }
    void TestFill() noexcept { S.TestFill(); }
    void DebugPrint() const noexcept {
        std::cout << "\nARRAY: "; S.DebugPrint();
        std::cout << "\nMATRIX VIEW:\n";
        std::cout << "ij";
        for(ui32 j=0; j<N; ++j) std::cout << ' ' << j;
        std::cout << '\n';
        for(ui32 i=0; i<N; ++i) {
            std::cout << i << ')';
            for(ui32 j=0; j<N; ++j) {
                std::cout << ' '; if (i<=j) std::cout <<  this->operator()(i,j); else std::cout << '.';
            }
            std::cout << '\n';
        }
    }
};

template<ui32 N, typename I=ui32>
class TPascalTriangle {
    TPyramid<N,I> S;
public:
    /* C(n,k) = (0/0) // from n choose k

              (0/0)
           (1/0) (1/1)
        (2/0) (2/1) (2/2)
      (3/0) (3/1)(3/2) (3/3)
     ........................

    or
              1
             1 1
            1 2 1
           1 3 3 1
          1 4 6 4 1

     */
    constexpr TPascalTriangle() {
        /*
        The attempt to do 'S(1,0) = S(2,0) = S(2,1) = 1;' in complile time
        caused an internal sigmentation fault inside g++(8.3), LOL)
        */
        S(1,0) = 1; S(2,0) = 1; S(2,1) = 1;
        for(ui32 l=3; l<=N; ++l) {
            S(l,0) = 1; S(l,l-1) = 1;
            for(ui32 o=1; o<l-1; ++o)
                S(l,o) = S(l-1,o)+S(l-1,o-1);
        }
    }
    constexpr I C(ui32 n, ui32 m) const noexcept {
        /*  number of combinations of m elements from n elements
                C(n,m) = n! / (m! * (n-m)!)
            Level  = n + 1
            Offset = m
            remember: o = [0,l)
        */
        return S(n+1,m);
    }

    void DebugPrint() const noexcept {
        std::cout << "\nARRAY: "; S.DebugPrint();
        std::cout << "\nPASCAL:\n";
        for(ui32 l=1; l<=N; ++l) {
            std::cout << 'L' << l << ')';
            for(ui32 o=0; o<l; ++o) {
                std::cout << ' ' << S(l,o);
            }
            std::cout << '\n';
        }
    }
};