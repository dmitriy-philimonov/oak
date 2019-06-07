#include "prefixtree.h"
#include <algorithm>
#include <iostream>

namespace NPrefix {
    /*
     * x always has '\0' at the end, it's very important here
     * 
     * Case 1: return key.size() < x.size()
     *   x='abcde\0'
     * key='abc'
     * 
     * Case 2: return key.size() == x.size()
     *   x='ab\0'
     * key='ab\0'
     * 
     * Case 3: return i=2 < key.size()
     *   x='ab\0'
     * key='abc'
     * 
     * Case 4: return i=2 < key.size()
     *   x='ab\0'
     * key='abcde\0'
     */
    size_t TTree::Prefix(const std::string_view x, const std::string_view key) const noexcept {
        // x always has '\0', key has only in leaf
        for(size_t i=0; i<key.size(); ++i)
            if (key[i] != x[i]) // if (x.size() < key.size()) '\0' in the 'x' must trigger this if
                return i;
        return key.size();
    }
    TNode* TTree::Split(TNode::TInner& parent, ui32 i) {
        TNode* child = new TNode();
        child->Keys.emplace_back(parent.Key.substr(i), parent.Link);

        parent.Key.resize(i); // there's no explicit '\0' any more here
        parent.Key.shrink_to_fit(); // I want to fit into SSO whenever it's possible
        parent.Link = child;
        return child;
    }

    struct TInnerFirstLetterCmp {
        bool operator ()(const TNode::TInner& lhs, const std::string_view& rhs) const noexcept {
            return lhs.Key.front() < rhs.front();
        }
        bool operator ()(const std::string_view& lhs, const TNode::TInner& rhs) const noexcept {
            return lhs.front() < rhs.Key.front();
        }
    };

    void TTree::AppendStrView(std::string_view x) {
        if (Root == nullptr) {
            Root = new TNode();
            Root->Keys.emplace_back(x); ++Size;
            return;
        }

        TNode* cur = Root;
        while(true) {
            auto& keys = cur->Keys;
            // main strength is here - O(log n) access via first letter => R-way compressed trie
            auto it = std::lower_bound(keys.begin(), keys.end(), x, TInnerFirstLetterCmp());
            if (it == keys.end()) {
                keys.emplace_back(x); ++Size;
                break;
            }
            std::string_view key = it->Key;
            ui32 i = Prefix(x, key);
            if (i == 0) {
                // main weakness is here - O(n) insert in a vector
                keys.emplace(it, x); ++Size;
                break;
            }
            if (i == x.size()) {
                // case 2 -> x is already in the tree
                break;
            }
            if (i == key.size()) {
                // case 1 -> key is a prefix of the x
                x.remove_prefix(i);
                cur = it->Link;
                continue;
            }
            // case 3,4 -> i < key.size()
            x.remove_prefix(i);
            cur = Split(*it, i);
        }
    }
    bool TTree::ExistsStrView(std::string_view x) const noexcept {
        if (Root == nullptr) return false;
        
        TNode* cur = Root;
        while(true) {
            auto& keys = cur->Keys;
            auto it = std::lower_bound(keys.begin(), keys.end(), x, TInnerFirstLetterCmp());
            if (it == keys.end()) return false;

            std::string_view key = it->Key;
            ui32 i = Prefix(x, key);
            if (i == 0) return false;
            if (i == x.size()) return true; // case 2 -> full match
            if (i == key.size()) {
                // case 1 -> key is a prefix of the x
                x.remove_prefix(i);
                cur = it->Link;
                continue;
            }
            // case 3,4 -> i < key.size()
            return false;
        }
    }
    void TTree::clear() noexcept {
        std::vector<TNode*> todo; todo.push_back(Root);
        while(!todo.empty()) {
            TNode* cur = todo.back(); todo.pop_back();
            for(auto& inner: cur->Keys)
                if (inner.Link)
                    todo.push_back(inner.Link);
            delete cur;
        }
        Root = nullptr;
    }

    struct TWithLevel {
        using TIt = std::vector<TNode::TInner>::iterator;
        TIt CurIt;
        TIt EndIt;
        ui32 L;
        TWithLevel(TIt c, TIt e, ui32 l)
            : CurIt(c)
            , EndIt(e)
            , L(l)
        {}
    };

    template<typename F>
    void InOrderTraverse(TNode& root, F visit) {
        std::vector<TWithLevel> todo; todo.emplace_back(root.Keys.begin(), root.Keys.end(), 1);
        while(!todo.empty()) {
            auto wl = todo.back(); todo.pop_back();
            auto nextCurIt = wl.CurIt; ++nextCurIt;
            if (nextCurIt != wl.EndIt)
                todo.emplace_back(nextCurIt, wl.EndIt, wl.L);
            if (wl.CurIt->Link)
                todo.emplace_back(wl.CurIt->Link->Keys.begin(), wl.CurIt->Link->Keys.end(), wl.L+1);
            visit(wl);
        }
    }

    TKeyRefs TTree::InOrder() const noexcept {
        TKeyRefs refs;
        if (!Root) return refs;
        InOrderTraverse(*Root, [&refs](const TWithLevel& wl){
            refs.push_back(wl.CurIt->Key);
        });
        return refs;
    }
    void TTree::DebugPrint() const noexcept {
        if (!Root) { std::cout << "Graph={}\n"; return; }

        std::cout << "Graph={\n";
        InOrderTraverse(*Root, [](const TWithLevel& wl) {
            ui32 l = wl.L; while(l--) std::cout << '-';
            std::string_view key = wl.CurIt->Key;
            if (key.back() == '\0') {
                key.remove_suffix(1);
                std::cout << key << "$\n";
            } else
                std::cout << key << '\n';
        });
        std::cout << "}\n";
    }
    void TTree::DebugInfo() const noexcept {
        if (!Root) return;
        struct TInfo {
            ui32 maxHeight = 0;
            ui32 nodesCount = 0;
        } info;
        InOrderTraverse(*Root, [&info](const TWithLevel& wl){
            ++info.nodesCount;
            info.maxHeight = std::max(info.maxHeight, wl.L);
        });
        std::cout << "Nodes=" << info.nodesCount << ", Height=" << info.maxHeight << '\n';
    }
}