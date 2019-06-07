#pragma once


#include "defines.h"
#include <cstring>
#include <stdexcept>
#include <vector>


namespace NMemory {
    struct TUtil {
        static inline size_t NearestPowerOfTwo(size_t size) noexcept {
            static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8,
                          "Unsupported machine architecture"
            );
            --size;
            size |= size >> 1;
            size |= size >> 2;
            size |= size >> 4;
            size |= size >> 8;  // => 2 bytes
            size |= size >> 16; // => 4 bytes
            if constexpr (sizeof(size_t) == 4) {
                ++size; return size;
            }
            size |= size >> 32; // => 8 bytes
            ++size; return size;
        }
    };
    struct TLinearStrategy {
        static inline size_t GetNextSize(const size_t size) noexcept {
            return size;
        }
    };
    struct TExponentialStrategy {
        static inline size_t GetNextSize(const size_t size) noexcept {
            return size << 1;
        }
    };

    template<typename TStrategy = TLinearStrategy>
    class TPool {
    private:
        static constexpr size_t DefaultBlockSize = 4096;
        struct TBlock {
            char* Ptr;
            size_t Size;
        };
        std::vector<TBlock> B;
        size_t Occupied = 0;
    private:
        size_t CurBlockSize() const noexcept {
            return B.back().Size;
        }
        char* CurBlockPtr() const noexcept {
            return B.back().Ptr;
        }
        void* CurPtr() const noexcept {
            return CurBlockPtr() + Occupied;
        }
        void AllocateBlock(const size_t size) {
            char* ptr = new char[size];
            B.push_back({ptr, size});
        }
        void Grow() {
            AllocateBlock(TStrategy::GetNextSize(CurBlockSize()));
            Occupied = 0;
        }
        void CheckSize(const size_t size) {
            size_t bs = CurBlockSize();
            if (size > bs)
                throw std::runtime_error("TPool: Current block size is not enough to store at least 1 element");
            if (Occupied + size > bs)
                Grow();
        }
    public:
        TPool() {
            AllocateBlock(DefaultBlockSize);
        }
        TPool(const size_t blockSize) {
            AllocateBlock(TUtil::NearestPowerOfTwo(blockSize));
        }
        void ReAllocate() {
            for(TBlock b: B)
                delete[] b.Ptr;
            
            size_t firstSize = B.front().Size;
            B.clear();

            AllocateBlock(firstSize);
            Occupied = 0;
        }
        ~TPool() {
            for(TBlock b: B)
                delete[] b.Ptr;
        }
        void* Append(const size_t size) {
            CheckSize(size);
            char* bytes = static_cast<char*>(CurPtr());
            Occupied += size;
            return bytes;
        }
        char* Append(const char* ptr, const size_t size) {
            CheckSize(size);
            
            char* strPtr = static_cast<char*>(CurPtr());
            std::memcpy(strPtr, ptr, size);

            Occupied += size;
            return strPtr;
        }
        template<size_t N>
        char* Append(const char(&cArray)[N]) {
            return Append(&cArray[0], N);
        }
        template<typename T>
        T* Append(const T& e) {
            CheckSize(sizeof(T));

            T* objPtr = static_cast<T*>(CurPtr());
            new (objPtr) T(e);

            Occupied += sizeof(T);
            return objPtr;
        } // calling destructor of object T is the user's responsibility

        size_t BlocksCount() const noexcept { return B.size(); }
    };
}