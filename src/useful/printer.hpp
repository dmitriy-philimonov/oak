#include <iostream>
#include <iomanip>
#include <string>


#ifndef NDEBUG
template<typename C, typename E=void>
void Print(const C& v) {
    std::cout << '{';
    if (v.empty()) {
        std::cout << "}\n";
        return;
    }
    for(const auto& e: v) {
        if constexpr (std::is_same_v<E,void>)
            std::cout << e << ',';
        else
            std::cout << E(e) << ',';
    }
    std::cout << "\b}\n";
}
template<typename E, typename C>
void PrintTyped(const C& v) {
    Print<C,E>(v);
}

template<typename TBiIterator>
void Print(TBiIterator begin, TBiIterator end) {
    auto it=begin; std::cout << '{';
    if (it == end) { std::cout << '}'; return; }
    else std::cout << *it;
    while(++it!=end) std::cout << ',' << *it;
    std::cout << "}\n";
};
template<typename T>
void Print(T* ptr, size_t n) {
    std::cout << '{';
    if(!n) {
        std::cout << "}\n";
        return;
    }
    for(size_t i=0; i<n; ++i) {
        std::cout << ptr[i] << ',';
    }
    std::cout << "\b}\n";
}

template<typename IOStream, typename T1, typename T2>
IOStream& operator << (IOStream& oss, const std::pair<T1,T2>& p) {
    oss << '[' << p.first << ',' << p.second << ']';
    return oss;
}

auto defaultGetter = [](const unsigned k){return k;};

template<typename C, typename E=void, typename G1 = decltype(defaultGetter), typename G2 = decltype(defaultGetter)>
void PrintMatrix(const C& M, G1 ig=defaultGetter, G2 jg=defaultGetter) {
    if (!M.empty()) {
        std::cout << ' ' << '|';
        for(unsigned j=0; j<M.front().size(); ++j) {
            std::cout << ' ' << jg(j);
        }
        std::cout << '\n';
    }
    for(unsigned i=0; i<M.size(); ++i) {
        const auto& row = M[i];
        std::cout << ig(i) << ')';
        for(unsigned j=0; j<row.size(); ++j) {
            std::cout << ' ';
            if constexpr (std::is_same_v<E,void>)
                std::cout << row[j];
            else
                std::cout << E(row[j]);
        }
        std::cout << '\n';
    }
}
template<typename E, typename C, typename G1 = decltype(defaultGetter), typename G2 = decltype(defaultGetter)>
void PrintMatrixTyped(const C& M, G1 ig=defaultGetter, G2 jg=defaultGetter) {
    PrintMatrix<C,E,G1,G2>(M,ig,jg);
}

void PrintMemory(void* m, size_t n) {
    using ui8 = uint8_t;
    using ui16 = uint16_t;
    using ui32 = uint32_t;

    std::cout << "\nA=" << m << " S=" << n << '\n';
    ui8* p = static_cast<ui8*>(m);
    ui8* e = p + n;

    ui32 i=0;
    std::cout << std::setfill('0') << std::hex;
    std::cout << "    ";
    while(i<16){
        if (i%8==0) std::cout << ' ';
        std::cout << ' ' << std::setw(2) << i++;
    }

    i=0;
    while (p < e) {
        if (i % 16 == 0) std::cout << '\n' << std::setw(4) << i;
        if (i++% 8 == 0) std::cout << ' ';
        std::cout << ' ' << std::setw(2) << (ui16)*p++;
    }
    std::cout << '\n' << '\n' << std::dec;
}

#define DEBUG(x) std::cout << #x << " = " << (x) << std::endl
#define DF(x) std::cout << #x << " =\n" << (x) << " <-- !!!\n"

#define PRINTABLE2(T,f1,f2)\
template<typename S>\
S& operator <<(S& oss,const T& t){\
    oss<<'['<<t.f1<<','<<t.f2<<']';return oss;\
}
#define PRINTABLE3(T,f1,f2,f3)\
template<typename S>\
S& operator <<(S& oss,const T& t){\
    oss<<'['<<t.f1<<','<<t.f2<<','<<t.f3<<']';return oss;\
}


#else
#define Print(...)
#define PrintMatrix(...)
#define PrintTyped(...)
#define PrintMatrixTyped(...)
#define PrintMemory(...)
#define DEBUG(...)
#define DF(...)
#define PRINTABLE(...)
#endif