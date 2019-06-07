#pragma once

/*
 * Realization of prefix tree based of deterministic finite automaton (dfa).
 * Basically, each prefix has it's own state, and character is a signal for moving from
 * one state to another.
 *   1. Yes, it consumes quite a lot of memory
 *   2. It surprisingly fast, amortized tests for real words look better than STL hash gives,
 * however, detailed research showed, that it's because of high frequency of short english words
 * in the input, which was, by the way, got from one very old and famous book.
 * 
 * For short words it's amazingly very fast, for long words - amazingly slow.
 * Some additional research should be conducted here, I reckon.
 */


#include "defines.h"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <vector>
#include <bitset>

namespace NPrefix {
    enum TConstants {
        EMPTY    = 0,
        HAS_WORD = 1,
    };

    using TMatrix = std::vector<std::vector<ui32>>;

    class TDfa {
    private:
        ui8 NextId = 0;
        ui32 NextState = EMPTY + 1;
        
        TMatrix M;
        ui32 Size=0;
    private:
        static constexpr ui8 FinalSymbol = 0;
        static constexpr ui16 AlphabetSize = 256;
        static constexpr ui32 DefaultSize = 1;

        ui8 XLAT[AlphabetSize];
        ui8 HAS_XLAT[AlphabetSize] = {0};

        ui8 DoXLAT(ui8 symbol) noexcept;
        void ExpandSV(std::vector<ui32>& sv, ui16 id);
        void ExpandM(TMatrix& m, ui32 newState);
    public:
        TDfa()
            : M(DefaultSize, std::vector<ui32>(1, EMPTY))
        {}
        bool insert(const std::string& x);
        bool exists(const std::string& x) const noexcept;
        ui32 size() const noexcept { return Size; }
        ui32 StateCount() const noexcept { return NextState; }

        void DebugPrint() const noexcept;
    };
}