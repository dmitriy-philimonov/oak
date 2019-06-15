#pragma once

/*
 * Realization of prefix tree based of deterministic finite automaton (dfa).
 * Basically, each prefix has it's own state, and character is a signal for moving from
 * one state to another.
 *   1. Yes, it consumes quite a lot of memory
 *   2. It surprisingly fast, amortized tests for real words look better than STL hash gives,
 * however, detailed research showed, that it's because of high frequency of short english words
 * in the input, which was, by the way, got from one very old and famous book.
 *   3. Frequent using of insert/erase commands reduces data locality, which affects cache performance and,
 * therefore, reduces total performance. The fastest search speed was achieved when storage was filled
 * with insert commands only and then tested.
 *
 * For short words it's amazingly very fast, for long words - amazingly slow.
 * Some additional research should be conducted here, I reckon.
 */


#include "defines.h"
#include <vector>
#include <string>

namespace NPrefix {
    enum TConstants {
        EMPTY    = 0,
        HAS_WORD = 1,
    };

    using TMatrix = std::vector<std::vector<ui32>>;
    using TStates = std::vector<ui32>;
    struct TVisit {
        ui32 CurState;
        ui32 CurId;
    };
    using TStateHistory = std::vector<TVisit>;

    class TDfa {
    private:
        ui8 NextId = 0;
        ui32 NextState = EMPTY + 1;

        TStates F;
        TMatrix M;
        ui32 Size=0;
    private:
        static constexpr ui8 FinalSymbol = 0;
        static constexpr ui16 AlphabetSize = 256;
        static constexpr ui32 DefaultSize = 1;

        ui8 XLAT[AlphabetSize];
        ui8 HAS_XLAT[AlphabetSize] = {0};

        ui8 DoXLAT(ui8 symbol) noexcept;
        ui32 GetNextState() noexcept;
        void UnfoldStateHistory(TStateHistory& sH);
        void ExpandSV(TStates& sv, ui16 id);
        void ExpandM(TMatrix& m, ui32 newState);
    public:
        TDfa()
            : M(DefaultSize, TStates(1, EMPTY))
        {}
        bool insert(const std::string& x);
        bool exists(const std::string& x) const noexcept;
        bool erase(const std::string& x) noexcept;
        ui32 size() const noexcept { return Size; }
        ui32 StateCount() const noexcept { return NextState; }

        void DebugPrint() const noexcept;
    };
}