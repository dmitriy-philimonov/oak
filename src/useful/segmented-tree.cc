#include <vector>

#include <iostream>
#include <iomanip>

/* for main() only */
#include "printer.hpp"

using i32 =int32_t;

template<typename IntType=i32>
class TSegmentedTree {
private:
    const size_t N;
    std::vector<IntType> Tree;

private:
    inline size_t L(size_t x) const noexcept { return 2*x; }
    inline size_t R(size_t x) const noexcept { return 2*x+1; }
    inline size_t P(size_t x) const noexcept { return x/2; }
    inline size_t isL(size_t x) const noexcept { return !(x & 0x1); }
    inline size_t isR(size_t x) const noexcept { return x & 0x1; }
    
public:
    TSegmentedTree(const std::vector<IntType>& data)
        : N(data.size())
    {
        Tree.resize(2*N);
        for(size_t i=N; i<2*N; ++i){
            Tree[i] = data[i-N];
        }
        for(size_t i=N-1; i>0; --i){
            Tree[i]=Tree[L(i)]+Tree[R(i)];
        }
        // Tree[0] is unused
    }
    void update(size_t i, IntType val) noexcept {
        i += N; Tree[i] = val;
        while(i>1){
            i=P(i);
            Tree[i]=Tree[L(i)]+Tree[R(i)];
        }
    }
    /* inclusive */
    IntType read(size_t b, size_t e) const noexcept {
        b+=N; e+=N;
        IntType sum = 0;
        while(b<=e){
            DEBUG(b); DEBUG(e);
            if(isR(b)) {DEBUG(Tree[b]);sum += Tree[b++];}
            if(isL(e)) {DEBUG(Tree[e]);sum += Tree[e--];}
            b /= 2; e /= 2;
        }
        return sum;
    }

    void debug_print() const noexcept{
        std::cout << "Segmented Tree [2*N=2*" << N
            << ", IntType=" << typeid(IntType).name()
            << "]\n";

        for(size_t i=0; i<2*N; ++i) {
            std::cout << std::setw(4) << i;
        }
        std::cout << '\n';
        for(size_t i=0; i<2*N; ++i) {
            std::cout << std::setw(4) << Tree[i];
        }
        std::cout << '\n'; std::cout.flush();
    }
};

int main() {
    TSegmentedTree<i32> tree({1,2,4,5,7,8,3,9});
    tree.debug_print();
    DF(tree.read(0,7));
    // tree.update(3, 1);
    // tree.update(0, 1);
    // tree.debug_print();
    return 0;
}