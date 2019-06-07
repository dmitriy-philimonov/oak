#pragma once

#include "defines.h"
#include <string>
#include <vector>

namespace NPrefix {
namespace NMemoryOptimized {

    enum TConstants {
        EMPTY    = 0,
        HAS_WORD = 1,
        WITH_END = 2,
    };
    union TU {
        ui32 NextState = EMPTY;
        struct {
            ui16 Status;
            ui16 Size;
        } Info;
        ui8 Ending[4];
        
        ui16 GetStatus() const noexcept { return Info.Status; }
        void SetStatus(ui16 status) { Info.Status = status; }
        ui16 GetSize() const noexcept { return Info.Size; }
        void SetSize(ui16 size) noexcept { Info.Size = size; }
        ui8* Storage() noexcept { return Ending; }
        const ui8* Storage() const noexcept { return Ending; }
    };
    static_assert(sizeof(TU) == 4);

    using TMatrixElement = std::vector<TU>;
    using TMatrix = std::vector<TMatrixElement>;
    using TEnding = std::vector<std::string>;

    class TDfa {
    private:
        ui8 NextId = 0;
        ui32 NextState = EMPTY + 1;
        
        TMatrix M;
        TEnding E;
        ui32 Size=0;
    private:
        static constexpr ui8 FinalSymbol = 0;
        static constexpr ui16 AlphabetSize = 256;
        static constexpr ui32 DefaultSize = 1;

        ui8 XLAT[AlphabetSize];
        ui8 HAS_XLAT[AlphabetSize] = {0};

        ui8 DoXLAT(ui8 symbol) noexcept;
        void SaveEnding(TMatrixElement& sv, const char* s, ui32 size);
        bool UnfoldMatchSymbols(const std::string& x, ui32 i, ui32 curState);
        bool UnforldMatchSymbolsFast(ui32 curState);
    public:
        TDfa()
            : M(DefaultSize, TMatrixElement(1))
        {}
        bool insert(const std::string& x);
        bool exists(const std::string& x) const noexcept;
        ui32 size() const noexcept { return Size; }

        void DebugPrint() const noexcept;
        ui32 StateCount() const noexcept { return NextState; }
    };
}
}