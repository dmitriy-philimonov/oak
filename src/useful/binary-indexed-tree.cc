#include <array>

/* for debug_print() only */
#include <iostream>
#include <iomanip>
/* for main() only */
#include "printer.hpp"


using u32=uint32_t;
using i32=int32_t;

/*
Theory: https://www.topcoder.com/thrive/articles/Binary%20Indexed%20Trees
*/

template <u32 MaxIdx=16, typename IntType=i32>
class TBinaryIndexedTree
{
private:
    /*
      If r is the position in idx of its last non-zero
      digit in binary notation, then:

      Tree[idx] holds the sum of frequencies for indices
      (idx - 2^r + 1) through idx, inclusive.
    */
    std::array<IntType, MaxIdx+1> Tree = {};
    static constexpr u32 most_significant_bit_of_MaxIdx() noexcept {
        u32 m = MaxIdx;
        m |= m >> 1;
        m |= m >> 2;
        m |= m >> 4;
        m |= m >> 8;
        m |= m >> 16;
        return m & ~(m >> 1);
    }
public:
    void update(u32 idx, IntType val) noexcept {
        while(idx <= MaxIdx) {
            Tree[idx] += val;
            idx += idx & (-idx);
        }
    }
    IntType read(u32 idx) const noexcept {
        IntType sum = 0;
        while(idx > 0) {
            sum += Tree[idx];
            idx -= idx & (-idx);
        }
        return sum;
    }

    IntType read_single(u32 idx) const noexcept {
        IntType sum = Tree[idx];
        if(idx == 0) return sum;

        u32 z = idx - (idx & (-idx));
        --idx;
        while(idx != z) {
            sum -= Tree[idx];
            idx -= idx & (-idx);
        }
        return sum;
    }

    /* inclusive [idx_b, idx_e]*/
    IntType read_range(u32 idx_b, u32 idx_e) const noexcept {
        if (idx_b > idx_e) return 0;
        if (idx_b == 0) return read(idx_e);

        IntType sum = 0; --idx_b;
        while(idx_e != idx_b) {
            if (idx_e > idx_b) {
                sum += Tree[idx_e];
                idx_e -= idx_e & (-idx_e);
            } else {
                sum -= Tree[idx_b];
                idx_b -= idx_b & (-idx_b);
            }
        }
        return sum;
    }

    u32 find_any(IntType target_sum) const noexcept {
        u32 idx = 0;
        u32 bit_mask = most_significant_bit_of_MaxIdx();

        while (bit_mask != 0) {
            u32 i = idx | bit_mask; bit_mask >>= 1;
            if (i > MaxIdx) continue;
            if (target_sum == Tree[i]) return i; // return the first fit
            else if (target_sum > Tree[i]) {
                // if the tree frequency "can fit" into target_sum,
                // then include it
                idx = i; target_sum -= Tree[i];
            }
        }
        return target_sum == 0 ? idx : u32(-1);
    }

    u32 find_greatest(IntType target_sum) const noexcept {
        u32 idx = 0;
        u32 bit_mask = most_significant_bit_of_MaxIdx();

        while (bit_mask != 0) {
            u32 i = idx | bit_mask; bit_mask >>= 1;
            if (i > MaxIdx) continue;
            if (target_sum >= Tree[i]) {
                // if the current cumulative frequency is equal to cumFre,
                // we are still looking for a higher index (if exists)
                idx = i; target_sum -= Tree[i];
            }
        }
        return target_sum == 0 ? idx : u32(-1);
    }
    
    void debug_print() const noexcept {
        std::cout << "Fenwick Tree [MaxIdx=" << MaxIdx
                  << ", IntType=" << typeid(IntType).name()
                  << "]\n";

        for(u32 idx=1; idx<=MaxIdx; ++idx) {
            std::cout << std::hex << std::setw(4) << idx;
        }
        std::cout << '\n';
        for(u32 idx=1; idx<=MaxIdx; ++idx) {
            std::cout << std::dec << std::setw(4) << Tree[idx];
        }
        std::cout << '\n'; std::cout.flush();
    }
};


int main() {
    TBinaryIndexedTree<> tree;
    tree.update(1, 1);
    tree.update(3, 1);
    tree.update(5, 1);

    tree.debug_print();
    DEBUG(tree.read_single(4));
    DEBUG(tree.read_single(5));
    DEBUG(tree.read_single(6));
    DEBUG(tree.find_greatest(2));
    DEBUG(tree.find_any(2));
    DEBUG(tree.read_range(3,6));

    return 0;
}
