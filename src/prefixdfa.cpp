#include "prefixdfa.h"

namespace NPrefix {
    ui8 TDfa::DoXLAT(ui8 symbol) noexcept {
        if (HAS_XLAT[symbol])
            return XLAT[symbol];
        ui8 id = NextId++;
        XLAT[symbol] = id;
        HAS_XLAT[symbol] = 1;
        return id;
    }
    void TDfa::ExpandSV(std::vector<ui32>& sv, ui16 id) {
        if (sv.size() <= id)
            sv.resize(id+1, EMPTY);
    }
    void TDfa::ExpandM(TMatrix& m, ui32 newState) {
        if (m.size() <= newState)
            m.resize(newState+1, std::vector<ui32>(1, EMPTY));
    }

    bool TDfa::insert(const std::string& x) {
        ui32 curState = EMPTY;
        for(char c: x) {
            ui16 id = DoXLAT(static_cast<ui8>(c))+1;
            auto& sv = M[curState];
            ExpandSV(sv, id);

            ui32 state = sv[id];
            if (state != EMPTY) {
                curState = state;
                continue;
            }

            state = NextState++;
            sv[id] = state;

            ExpandM(M, state);
            curState = state;
        }
        ui32& end = M[curState][FinalSymbol];
        bool r = end == EMPTY; // this word is new
        end = HAS_WORD; Size += r;
        return r;
    }
    bool TDfa::exists(const std::string& x) const noexcept {
        ui32 curState = EMPTY;
        for(char c: x) {
            ui8 symbol = static_cast<ui8>(c);
            if (!HAS_XLAT[symbol]) return false;
            ui16 id = XLAT[symbol]+1;

            const auto& sv = M[curState];
            if (sv.size() <= id) return false;
            ui32 state = sv[id];
            if (state == EMPTY) return false;

            curState = state;
        }
        return M[curState][FinalSymbol] == HAS_WORD;
    }
    void TDfa::DebugPrint() const noexcept {
        ui8 RXLAT[AlphabetSize];
        ui16 RLen = 0;
        for(ui16 symbol=0; symbol<AlphabetSize; ++symbol)
            if (HAS_XLAT[symbol]) {
                ++RLen; RXLAT[XLAT[symbol]] = symbol;
            }

        std::cout << std::setw(3) << "id" << ") $";
        for(ui16 id=0; id<RLen; ++id)
            std::cout << ' ' << RXLAT[id];
        std::cout << '\n';

        for(ui32 state=0; state<M.size(); ++state) {
            std::cout << std::setw(3) << state << ')';
            auto& sv = M[state];
            for(ui8 id=0; id<sv.size(); ++id) {
                std::cout << ' ' << sv[id];
            }
            std::cout << '\n';
        }
    }
}