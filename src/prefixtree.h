#pragma once

/*
 *  R-way compressed trie / prefix tree
 *  1. Insert is quite slow, insert in the middle of a vector takes place: O(n)
 *  2. Search is quite fast
 *  3. Memory sufficient approach:
 *   - on big inputs std::string uses so called 'small string optimization'
 *   - R-way sorted vector of partial keys in each node
 * P.S. Currently it supports only null-terminated strings (realization is based on that fact)
 * P.P.S. Due to a specific application of this prefix tree, the interface is customized (not STL-like)
 *   TTree tree;
 *   tree.Append("abc");
 *   tree.Exists("bcd");
 *   tree.Remove("abc");
 *   for(auto it=tree.AllKeys();it;++it)
 *       std::cout << it.Key() << '\n';
 */

#include "defines.h"
#include <string>
#include <vector>


namespace NPrefix {
    struct TNode {
        struct TInner {
            std::string Key;
            TNode* Link;
            // for Append
            TInner(std::string_view key)
                : Key(key)
                , Link(nullptr)
            {}
            // for Split
            TInner(std::string&& key, TNode* link)
                : Key(key)
                , Link(link)
            {}
        };
        std::vector<TInner> Keys;
    };
    using TKeyRefs = std::vector<std::string_view>;
    using TKeyIt = std::vector<TNode::TInner>::iterator;
    using TCKeyIt = std::vector<TNode::TInner>::const_iterator;

    class TTree;
    class TIterator {
    private:
        struct TUnit {
            std::string P;
            TCKeyIt Begin;
            TCKeyIt End;

            TUnit(const std::string& p, TCKeyIt b, TCKeyIt e)
                : P(p), Begin(b), End(e)
            {}
            TUnit(std::string&& p, TCKeyIt b, TCKeyIt e)
                : P(std::move(p)), Begin(b), End(e)
            {}
        };
        /*
           1. The last S element is used to store the current leaf to show via Key() method
           2. For other cases S is used as:
                -> Prefix is an accumulated string for [Begin,End) key ranges
                -> Prefix + Begin->Key is actual value for a leaf, or a new Prefix for a subtree

            Usage:
                TTree tree; // ... fill tree ...
                for(auto it = tree.KeysWithPrefix("pre"); it; ++it)
                    std::cout << it.Key() << '\n';
                }

         */
        std::vector<TUnit> S;
        const TTree* T;
    private:
        void GoDownToLeaf(std::string p, TCKeyIt b);
    public:
        TIterator() : T(nullptr) {}
        TIterator(const TTree* tree);
        TIterator(std::string_view x, const TTree* tree);
        std::string Key() const noexcept { return S.back().P; }
        operator bool() const noexcept { return !S.empty(); }
        TIterator& operator ++() noexcept;
        TIterator& operator=(TIterator&& other) {
            S = std::move(other.S);
            T = other.T;
            return *this;
        }
    };

    class TTree {
    private:
        TNode Root;
        ui32 Size = 0;
    private:
        size_t Prefix(const std::string_view x, const std::string_view key) const noexcept;
        TNode* Split(TNode::TInner& parent, ui32 i);
        void Join(TNode* cur, TKeyIt parent);
        bool AppendStrView(std::string_view x);
        bool ExistsStrView(std::string_view x) const noexcept;
        bool RemoveStrView(std::string_view x);
    public:
        TTree(){}
        ~TTree() {
            clear();
        }
        bool Append(const std::string& s) {
            // copy '\0' too, it's very important
            return AppendStrView(std::string_view(s.c_str(), s.size()+1));
        }
        bool Exists(const std::string& x) const noexcept {
            // copy '\0' too, it's very important
            return ExistsStrView(std::string_view(x.c_str(), x.size()+1));
        }
        bool Remove(const std::string& s) {
            return RemoveStrView(std::string_view(s.c_str(), s.size()+1));
        }
        TIterator KeysWithPrefix(const std::string& s) const noexcept {
            return TIterator(std::string_view(s.c_str(), s.size()+1), this);
        }
        TIterator AllKeys() const noexcept {
            return TIterator(this);
        }

        template<size_t N>
        bool Append(const char(&s)[N]) {
            // Is it a null-terminated C-string?
            if (s[N-1] == '\0')
                return AppendStrView(std::string_view(&s[0], N));
            // oh... pretend it is!
            std::string x(&s[0], N);
            return AppendStrView(std::string_view(x.c_str(), x.size()+1));
        }

        template<size_t N>
        bool Exists(const char(&x)[N]) {
            if (x[N-1] == '\0')
                return ExistsStrView(std::string_view(&x[0], N));

            std::string xs(&x[0], N);
            return ExistsStrView(std::string_view(xs.c_str(), xs.size()+1));
        }
        void DebugPrint() const noexcept;
        void DebugInfo() const noexcept;
        TKeyRefs InOrder() const noexcept;
        void clear() noexcept;
        ui32 size() const noexcept { return Size; }

        friend class TIterator;
    };
}