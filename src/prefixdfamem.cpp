#include "prefixdfamem.h"
#include <cstring>
#include <iostream>
#include <iomanip>


namespace NPrefix {
namespace NMemoryOptimized {
    ui8 TDfa::DoXLAT(ui8 symbol) noexcept {
        if (HAS_XLAT[symbol])
            return XLAT[symbol];
        ui8 id = NextId++;
        XLAT[symbol] = id;
        HAS_XLAT[symbol] = 1;
        return id;
    }
    inline void ExpandSV(TMatrixElement& sv, ui16 id)  {
        if (sv.size() <= id)
            sv.resize(id+1);
    }
    void TDfa::SaveEnding(TMatrixElement& sv, const char* s, ui32 ssize) {
            sv.resize(2+ssize/4); // 1 - info, 1+size/4 - for data
            TU* tu = &sv[FinalSymbol];
            tu->SetStatus(WITH_END);
            tu->SetSize(ssize);
            ++tu;
            std::memcpy(tu->Storage(), s, ssize);
    }

    bool TDfa::UnforldMatchSymbolsFast(ui32 curState) {
        // copy chars to nearest neighbour if vector capacity is enough, 
        // overwise all references become dead ...
        if (M.capacity() == M.size())
            return false;
        auto& sv = M[curState];
        TU& info = sv[FinalSymbol];
        TU& data = sv[FinalSymbol+1];
        
        info.SetStatus(EMPTY);
        ui32 nextState = NextState++;
        // if this routine is called, this shouldn't trigger a vector memory reallocation
        M.resize(NextState, TMatrixElement(1));
        if (info.GetSize() == 1)
            M[nextState][FinalSymbol].SetStatus(HAS_WORD);
        else
            SaveEnding(M[nextState], (char*)data.Storage()+1, info.GetSize()-1);
        sv.resize(1); // only future constructor, which sets memory to EMPTY
        
        ui16 id = DoXLAT(*data.Storage())+1;
        sv.resize(id+1);              // overwrite old ending string with EMPTY value
        sv[id].NextState = nextState; // and then set ref to a new state

        return true;
    }

    bool TDfa::UnfoldMatchSymbols(const std::string& x, ui32 i, ui32 curState) {
        TU& info = M[curState][FinalSymbol];
        TU& data = M[curState][FinalSymbol+1];

        // check they aren't equal
        if (x.size()-i == info.GetSize()) {
            ui8* p = data.Storage();
            bool theSame = true;
            for(ui32 j=i; j<x.size(); ++j) {
                if (*p++ != x[j]) {
                    theSame = false; break;
                }
            }
            if (theSame) return false;
        }

        // quite often situation
        if (x[i] != *data.Storage() && UnforldMatchSymbolsFast(curState))
            return true;

        // middle copy of ending (we need to overwrite ending memory)
        std::string ending((char*)data.Storage(), info.GetSize());
        info.SetStatus(EMPTY);
        M[curState].resize(1);

        ui16 j=0;
        while(j<ending.size()) {
            auto& sv = M[curState];
            ui16 id = DoXLAT(static_cast<ui8>(ending[j]))+1;
            sv.resize(id+1);
            curState = NextState++;
            sv[id].NextState = curState;
            M.resize(NextState, TMatrixElement(1));

            // always inc j, there's a condition later
            ++j;
            // if the last symbol in x is matched,
            // I need to process one more symbol from j,
            // if it's still available
            if (i>=x.size() || x[i++] != ending[j-1])
                break;
        }
        if (j == ending.size()) {
            M[curState][FinalSymbol].SetStatus(HAS_WORD);
            return true;
        }
        SaveEnding(M[curState], ending.data()+j, ending.size()-j);
        return true;
    }

    bool TDfa::insert(const std::string& x) {
        ui32 curState = EMPTY;
        ui32 i=0;
        while(i<x.size()) {
            // unfold old ending if it's here
            if (M[curState][FinalSymbol].GetStatus() == WITH_END)
                if (!UnfoldMatchSymbols(x, i, curState))
                    return false;

            auto& sv = M[curState];

            ui16 id = DoXLAT(static_cast<ui8>(x[i]))+1;
            ExpandSV(sv, id);

            curState = sv[id].NextState; ++i;
            if (curState != EMPTY)
                continue;
            
            ui32 newState = NextState++;
            sv[id].NextState = newState;

            M.resize(NextState, TMatrixElement(1));
            if (i == x.size()) {
                M[newState][FinalSymbol].SetStatus(HAS_WORD);
                ++Size; return true;
            }

            // save rest to the endings, even one letter
            // storage is reversed
            ui32 ssize = x.size() - i;
            if (__builtin_expect(ssize >= std::numeric_limits<ui16>::max(), 0))
                throw std::runtime_error("Unsupported so long strings, but can be added quite easy");

            SaveEnding(M[newState], &x[i], ssize);
            ++Size; return true;
        }
        auto& info = M[curState][FinalSymbol];
        switch (info.GetStatus()) {
            case EMPTY:
                info.SetStatus(HAS_WORD);
                ++Size; return true;
            case HAS_WORD:
                return false;
            case WITH_END:
                // shift one symbol from ending
                if (!UnforldMatchSymbolsFast(curState))
                    UnfoldMatchSymbols(x, x.size(), curState);
                // M is modified
                M[curState][FinalSymbol].SetStatus(HAS_WORD);
                ++Size; return true;
            default:
                return false;
        }
    }
    bool TDfa::exists(const std::string& x) const noexcept {
        ui32 curState = EMPTY;
        for(ui32 i=0; i<x.size(); ++i) {
            const auto& sv = M[curState];
            const auto& info = sv[FinalSymbol];
            if (info.GetStatus() == WITH_END) {
                const auto& data = sv[FinalSymbol+1];
                std::string_view ending((char*)data.Storage(), info.GetSize());
                std::string_view restOfX(&x[i], x.size()-i);
                return ending == restOfX;
            }

            ui8 symbol = static_cast<ui8>(x[i]);
            if (!HAS_XLAT[symbol]) return false;
            ui16 id = XLAT[symbol]+1;
            
            if (sv.size() <= id) return false;
            ui32 state = sv[id].NextState;
            if (state == EMPTY) return false;

            curState = state;
        }
        return M[curState][FinalSymbol].GetStatus() == HAS_WORD;
    }
    void TDfa::DebugPrint() const noexcept {
        ui8 RXLAT[AlphabetSize];
        ui16 RLen = 0;
        for(ui16 symbol=0; symbol<AlphabetSize; ++symbol)
            if (HAS_XLAT[symbol]) {
                ++RLen; RXLAT[XLAT[symbol]] = symbol;
            }
        
        std::cout << '\n' << std::setw(3) << "id" << ") $";
        for(ui16 id=0; id<RLen; ++id)
            std::cout << ' ' << std::setw(3) << RXLAT[id];
        std::cout << '\n';

        for(ui32 state=0; state<M.size(); ++state) {
            std::cout << std::setw(3) << state << ')';
            const auto& sv = M[state];
            const auto& info = sv[FinalSymbol];

            switch(info.GetStatus()) {
                case EMPTY: std::cout << " E"; break;
                case HAS_WORD: std::cout << " H"; break;
                case WITH_END: {
                    const auto& data = sv[FinalSymbol+1];
                    std::cout << "WE(" << info.GetSize() << "):" << (char*)data.Storage() << '\n';
                    continue;
                }
            }

            for(ui8 id=1; id<sv.size(); ++id) {
                std::cout << ' ' << std::setw(3) << sv[id].NextState;
            }
            std::cout << '\n';
        }
    }
}
}