#pragma once

/*  AVL tree.

P.S. Insert only supported, so instead of operator new a 'deque-based allocator' used).
   This is a canonic realization, from what I heard.
   The realization was developed for hackerrank.com task and tested by it.
   Published here for theoretical purposes: benchmark tests only.
*/

#include "defines.h"
#include <iostream>
#include <deque>


namespace NAvl {

    template<typename TKey>
    class TTree {
    private:
        struct TNode {
            TNode* Left = nullptr;
            TNode* Right = nullptr;
            int Ht = 0;
            TKey Val;
            TNode(const TKey& val) : Val(val) {}
            void DebugPrint() {
                std::cout << '(' << Val << ',' << Ht << ',' << TTree::BF(this) << ')' << '\n';
            }
        };
    private:
        std::deque<TNode> S;
        TNode* Register(const TKey& val) {
            S.push_back(TNode{val});
            return &S.back();
        }
    private:
        TNode* Root = nullptr;
    private:
        /* current height */
        static int H(TNode* node) noexcept {
            if (node == nullptr)
                return -1;
            return node->Ht;
        }
        /* calculate height */
        static int CalcH(TNode* node) noexcept {
            return std::max(H(node->Left) + 1, H(node->Right) + 1);
        }
        /* balance factor */
        static int BF(TNode* node) noexcept {
            return H(node->Left) - H(node->Right);
        }
        TNode* BalanceLeft(TNode* node) {
            TNode* pp = node;
            TNode* p = node->Left;
            TNode* c = p->Left; //case 2
            if (BF(p) < 0) {
                // small RL
                c = p->Right;   //case 1 -> case 2
                pp->Left = c;
                p->Right = c->Left;
                c->Left = p;

                --p->Ht;
                ++c->Ht;
                std::swap(p, c);
            }

            // big RR
            pp->Left = p->Right;
            p->Right = pp;
            pp->Ht-=2;
            return p;
        }
        TNode* BalanceRight(TNode* node) {
            TNode* pp = node;
            TNode* p = node->Right;
            TNode* c = p->Right; // case 4
            if (BF(p) > 0) {
                // small RR
                c = p->Left; // case 3 -> case 4
                pp->Right = c;
                p->Left = c->Right;
                c->Right = p;

                --p->Ht;
                ++c->Ht;
                std::swap(p, c);
            }

            // big RL
            pp->Right = p->Left;
            p->Left = pp;
            pp->Ht-=2;
            return p;
        }

        // BF = H(L) - H(R)
        TNode* InsImpl(TNode* node, const TKey& val) {
            if (node == nullptr)
                return Register(val);

            if (val <= node->Val) {
                node->Left = InsImpl(node->Left, val);
                node->Ht = CalcH(node);
                if (BF(node) > 1)
                    return BalanceLeft(node);
                return node;
            }

            node->Right = InsImpl(node->Right, val);
            node->Ht = CalcH(node);
            if (BF(node) < -1)
                return BalanceRight(node);
            return node;
        }
    public:
        void Insert(const TKey& val) {
            Root = InsImpl(Root, val);
        }
        bool Member(const TKey& val) const noexcept {
            TNode* cur = Root;
            while(cur != nullptr) {
                if (val < cur->Val) {
                    cur = cur->Left;
                    continue;
                }
                if (cur->Val < val) {
                    cur = cur->Right;
                    continue;
                }
                // val == cur->Val
                return true;
            }
            return false;
        }

        void DebugPrint() const noexcept {
            if (!Root) return;
            std::cout << "R: "; Root->DebugPrint();
            std::cout << "DFS\n";
            std::deque<TNode*> todo; todo.push_back(Root);
            while(!todo.empty()) {
                TNode* cur = todo.back(); todo.pop_back();
                cur->DebugPrint();
                if(cur->Right) todo.push_back(cur->Right);
                if(cur->Left) todo.push_back(cur->Left);
            }
        }
    };

} // NAvl