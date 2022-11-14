#pragma once


#include <array>
#include <memory>
#include <cstring>


/* cache for max 64K elements */
template<typename PodType, uint16_t N, typename allocator=std::allocator<PodType>>
class TCache64 {
private:
    using u16 = uint16_t;
    std::array<PodType, N> Store;
    std::array<u16, N> Idx; /* overhead max 128K */
    u16 I; allocator Alloc;

public:
    constexpr TCache64()
        : I(N)
    {
        u16 i=N, j=0;
        while(i--) Idx[j++]=i;
    }
    PodType* Allocate() {
        if (I) {
            return &Store[Idx[--I]];
        }
        return Alloc.allocate(1);
    }
    void Deallocate(PodType *ptr) {
        if (&Store[0] <= ptr && ptr <= &Store[N-1]) {
            std::memset(ptr, 0, sizeof(*ptr));
            Idx[I++] = ptr - &Store[0] + 1;
            return;
        }
        Alloc.deallocate(ptr);
    }
};