#pragma once

/*
   van Emde Boas tree

   Realization is got from the famous CLRS book (and rewritten in C++ with some template magic)

   Complexity of insert/delete/predecessor/successor operations is O(log log U),
   where U (the nearest larger power of 2) is the amount of integers [0..U-1] which the keys belong to.
   e.g. U = 2^32, K = log U = 32, O(log log U) = log 32 = 5
   P.S. Minimum/Maximum
 */

#include "defines.h"
#include <iostream>


namespace NvanEmdeBoas{

    static constexpr ui32 NIL = std::numeric_limits<ui32>::max();

    static inline std::string Val(ui32 v) {
        return v == NIL ? "NIL" : std::to_string(v);
    };
    static inline void MakeIndent(ui32 indent) {
        while(indent--) std::cout << '-';
    }


    template<ui32 K>
    class TvEB {
        /* for K = 31 it consumes around 21G, although operations do recursion no more than 5 times */
        static_assert(K <= 23, "G++ 8.1 hangs more than 10s creating a sequence of -1, poor asm generator ...");
    private:
        static constexpr ui32 U     = 0x1UL << K;
        static constexpr ui32 LSU_K = K/2;
        static constexpr ui32 HSU_K = (K + K%2)/2;
        static constexpr ui32 LSU   = 0x1UL << LSU_K;
        static constexpr ui32 HSU   = 0x1UL << HSU_K;

        TvEB<HSU_K> S;      //summary
        TvEB<LSU_K> C[HSU]; //cluster

        ui32 MIN=NIL;
        ui32 MAX=NIL;

    public:
        /* utility */

        ui32 High(ui32 x) const noexcept {
            return x >> LSU_K;
        }
        ui32 Low(ui32 x) const noexcept {
            return x & (LSU-1);
        }
        ui32 Index(ui32 h, ui32 l) const noexcept {
            return (h << LSU_K) | l;
        }

    public:
        /* operations */

        /* O(1) */
        ui32 Minimum() const noexcept { return MIN; }
        /* O(1) */
        ui32 Maximum() const noexcept { return MAX; }

        /* O(log log U) */
        bool Member(ui32 x) const noexcept {
            if (x == MIN || x == MAX) return true;
            return C[High(x)].Member(Low(x));
        }
        /* O(log log U) */
        ui32 Successor(ui32 x) const noexcept {
            if (MIN != NIL && x < MIN) return MIN;
            ui32 highX = High(x);
            ui32 lowX = Low(x);

            ui32 maxLow = C[highX].Maximum();
            if (maxLow != NIL && lowX < maxLow)
                return Index(highX, C[highX].Successor(lowX));

            ui32 succCluster = S.Successor(highX);
            if (succCluster == NIL) return NIL;
            return Index(succCluster, C[succCluster].Minimum());
        }
        /* O(log log U) */
        ui32 Predecessor(ui32 x) const noexcept {
            if (MAX != NIL && x > MAX) return MAX;
            ui32 highX = High(x);
            ui32 lowX = Low(x);

            ui32 minLow = C[highX].Minimum();
            if(minLow != NIL && lowX > minLow)
                return Index(highX, C[highX].Predecessor(lowX));

            ui32 predCluster = S.Predecessor(highX);
            if (predCluster == NIL) {
                if (MIN != NIL && x > MIN) return MIN;
                return NIL;
            }
            return Index(predCluster, C[predCluster].Maximum());
        }
        /* O(1) */
        bool EmptyInsert(ui32 x) noexcept {
            MIN = MAX = x; return true;
        }
        /* O(log log U) */
        bool Insert(ui32 x) noexcept {
            if (MIN == NIL)
                return EmptyInsert(x);

            if (x == MIN) return false;
            if (x < MIN) std::swap(x, MIN);
            if (x > MAX) MAX = x;
            ui32 highX = High(x);
            ui32 lowX = Low(x);
            bool r = false;
            auto& c = C[highX];
            if (c.Minimum() == NIL) {
                r = S.Insert(highX);
                c.EmptyInsert(lowX);
            } else
                r = c.Insert(lowX);
            return r;
        }
        /* O(log log U) */
        bool Delete(ui32 x) noexcept {
            if (MIN == MAX) {
                if (x == MIN) {
                    MIN = MAX = NIL;
                    return true;
                }
                return false;
            }
            if (x == MIN) {
                ui32 firstCluster = S.Minimum();
                x = Index(firstCluster, C[firstCluster].Minimum());
                MIN = x;
            }
            ui32 highX = High(x);
            ui32 lowX = Low(x);
            auto& c = C[highX];
            if (!c.Delete(lowX)) return false;
            if (c.Minimum() == NIL) {
                S.Delete(highX);
                if (x == MAX) {
                    ui32 summaryMax = S.Maximum();
                    MAX = summaryMax == NIL ? MIN : Index(summaryMax, C[summaryMax].Maximum());
                }
                return true;
            }
            if (x == MAX) MAX = Index(highX, c.Maximum());
            return true;
        }

        void DebugPrint(ui32 indent=0) const noexcept {
            MakeIndent(indent); std::cout << "{U" << U << " MIN:" << Val(MIN) << " MAX:" << Val(MAX) << '\n';
            MakeIndent(indent); std::cout << "S:\n"; S.DebugPrint(indent+1);
            for(ui32 i=0; i<HSU; ++i) {
                MakeIndent(indent); std::cout << 'C' << i << ':' << '\n';
                C[i].DebugPrint(indent+1);
            }
            MakeIndent(indent); std::cout << "}\n";
        }
    };

    template<>
    class TvEB<1> {
    private:
        ui32 MIN=NIL;
        ui32 MAX=NIL;

        static constexpr ui32 U = 2;
    public: /* O(1) all */
        bool Member(ui32 x) const noexcept {
            return x == MIN || x == MAX;
        }
        ui32 Minimum() const noexcept { return MIN; }
        ui32 Maximum() const noexcept { return MAX; }
        ui32 Successor(ui32 x) const noexcept {
            if (x==0 && MAX == 1) return 1;
            return NIL;
        }
        ui32 Predecessor(ui32 x) const noexcept {
            if (x==1 && MIN == 0) return 0;
            return NIL;
        }
        bool EmptyInsert(ui32 x) {
            MIN = MAX = x; return true;
        }
        bool Insert(ui32 x) noexcept {
            if (MIN==NIL) {
                MIN = MAX = x;
                return true;
            }
            if (x < MIN) { MIN = x; return true; }
            if (x > MAX) { MAX = x; return true; }
            return false;
        }
        bool Delete(ui32 x) noexcept {
            /* possible values here:
                MIN MAX
                0   0
                0   1
                1   1
            --- 1 - 0 --- impossible (see Insert)
            */
            if (MIN == MAX) {
                if (x == MIN) {
                    MIN = MAX = NIL;
                    return true;
                }
                return false;
            }
            // MIN = 0 MAX = 1, x = 0 or 1
            MIN = !x;
            MAX = MIN;
            return true;
        }
        void DebugPrint(ui32 indent=0) const noexcept {
            MakeIndent(indent); std::cout << "{U" << U << " MIN:" << Val(MIN) << " MAX:" << Val(MAX) << "}\n";
        }
    };


    template<ui32 MaxVal>
    class TTree {
    private:
        static_assert(MaxVal > 0, "Invalid MaxVal");
        static constexpr ui32 MaxPowerOf2(ui32 val) {
            return 32 - __builtin_clz(val);
        }

    private:
        static constexpr ui32 K = MaxPowerOf2(MaxVal);
        TvEB<K> Tree;

    public:
        TTree() {
            /* To avoid gcc bug and get more than 2^^23 unsigned integers
            to work with, of course if you have enough RAM (a theoretical research):
                1. Remove all MIN=NIL/MAX=NIL from TvEB<K> structures
                2. Run here std::memset(&Tree, 0xff, sizeof(Tree));
            P.S. It works, tested at 2^^31!
            */
        }
        ui32 Minimum() const noexcept { return Tree.Minimum(); }
        ui32 Maximum() const noexcept { return Tree.Maximum(); }
        ui32 Predecessor(ui32 x) const noexcept { return Tree.Predecessor(x); }
        ui32 Successor(ui32 x) const noexcept { return Tree.Successor(x); }
        bool Insert(ui32 x) noexcept {
            if (x > MaxVal) return false;
            return Tree.Insert(x);
        }
        bool Delete(ui32 x) noexcept {
            if (x > MaxVal) return false;
            return Tree.Delete(x);
        }
        void InsertRange(std::initializer_list<ui32> l) noexcept {
            for (ui32 i: l) Insert(i);
        }
        void DebugPrint() const noexcept { return Tree.DebugPrint(); }
    };

} //NvanEmdeBoas