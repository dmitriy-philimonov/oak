#pragma once

/*
 *  R-way compressed trie / prefix tree
 *  1. Insert is quite slow, insert in the middle of a vector takes place: O(n)
 *  2. Search is very fast
 *  3. Memory sufficient approach:
 *   - on big inputs std::string uses so called 'small string optimization'
 *   - R-way sorted vector of partial keys in each node
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

    class TTree {
    private:
        TNode* Root = nullptr;
        ui32 Size = 0;
    private:
        size_t Prefix(const std::string_view x, const std::string_view key) const noexcept;
        TNode* Split(TNode::TInner& parent, ui32 i);
        void AppendStrView(std::string_view x);
        bool ExistsStrView(std::string_view x) const noexcept;
    public:
        TTree(){}
        ~TTree() {
            clear();
        }
        void Append(const std::string& s) {
            // copy '\0' too, it's very important
            return AppendStrView(std::string_view(s.c_str(), s.size()+1));
        }
        bool Exists(const std::string& x) const noexcept {
            // copy '\0' too, it's very important
            return ExistsStrView(std::string_view(x.c_str(), x.size()+1));
        }
        template<size_t N>
        void Append(const char(&s)[N]) {
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
    };
}