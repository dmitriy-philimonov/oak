#pragma once


#include <cmath>
#include <iostream>
#include <vector>


/*
 * "Introduction to Algorithms, third edition" by
 *     Thomas H. Cormen
 *     Charles E. Leiserson
 *     Ronald L. Rivest
 *     Clifford Stein
 *
 * Chapter 19 Fibonacci Heaps (page 505)
 * 
 * Asymptotics:
 *   - Insert       O(1)
 *   - Minimum      O(1)
 *   - Extract-Min  O(lg n)
 *   - Union        O(1)
 *   - Decrease-Key O(1)
 *   - Delete       O(lg n)
 */


using i32 = int32_t;
using u32 = uint32_t;

template<typename IntType=i32>
struct TFibonacciNode {
    using TNode = TFibonacciNode;
    TNode* l; // left
    TNode* r; // right
    TNode* p; // parent
    TNode* child; 
    /* 
    * degree = number of children;
    * mark = this node has lost child 
        since last time this node was
        made a child of another node.
    */
    u32 mark:1, degree:31;
    IntType key;
};


template<typename IntType>
class TFibonacciMinHeap {
    using TNode = TFibonacciNode<IntType>;
private:
    TNode *Min;
    u32 N;

    static constexpr double log_phi = std::log((1 + std::sqrt(5))/2);
private:
    void AddToList(TNode* dst, TNode *x) noexcept {
        /* to left from Min */
        x->l = dst->l; x->l->r = x;
        x->r = dst; dst->l = x;
    }

    u32 Dn() const noexcept {
        double n = N;
        return std::log(n) / log_phi;
    }

    void FibHeapLink(TNode *y, TNode *x) noexcept {
        if (!x->child) {
            x->child=y; y->l=y; y->r=y;
        } else {
            AddToList(x->child, y);
        }
        y->p=x; ++x->degree; y->mark=false;
    }

    /* O(D(n)) = O(ln n) */
    void Consolidate() noexcept {
        u32 a_size = Dn() + 1;
        std::vector<TNode *> A(a_size);
        TNode *w = Min;
        do{
            TNode *x = w; w = w->l;
            u32 d = x->degree; /* x->{l,r} are broken */
            while(A[d] != nullptr){
                TNode *y = A[d];
                if (x->key > y->key) std::swap(x,y);
                FibHeapLink(y,x);
                A[d]=nullptr; ++d;
            }
            A[d] = x;
        }while(w != Min);
        
        Min=nullptr; /* x->{l,r} are fixed */
        for(u32 i=0; i<a_size; ++i) {
            TNode *x = A[i];
            if (x == nullptr) continue;
            if (Min == nullptr) {
                x->l=x; x->r=x; Min=x;
            } else {
                AddToList(Min, x);
                if (x->key < Min->key) Min = x;
            }
        }
    }

    void Cut(TNode *x, TNode *y) {
        /* remove x from child list of y */
        if (x == x->l) {
            y->child = nullptr;
        } else {
            if (y->child == x) y->child=x->l;
            x->l->r = x->r;
            x->r->l = x->l;
        }
        /* move x to the root list */
        --y->degree; AddToList(Min, x); x->p=nullptr; x->mark=false;
    }
    void CascadingCut(TNode *y) {
        TNode *z=y->p;
        if(z == nullptr) return;
        if(y->mark == false) y->mark=true;
        else {
            Cut(y, z);
            CascadingCut(z);
        }
    }
public:
    TFibonacciMinHeap() // MakeHeap
        : Min(nullptr)
        , N(0)
    {}

    bool Empty() const noexcept { return N == 0; }
    void Insert(TNode *x) noexcept {
        if (x == nullptr) return;
        x->p = nullptr;
        x->child = nullptr;
        x->degree = 0;
        x->mark = false;
        if (Min == nullptr) {
            x->l = x; x->r = x;
            Min = x;
        }
        else {
            AddToList(Min, x);
            if (x->key < Min->key) Min = x;
        }
        ++N;
    }
    TNode* Minimum() const noexcept {
        return Min;
    }
    TNode* ExtractMin() {
        if (Min == nullptr) return nullptr;
        TNode *z = Min;
        if(z->child) {
            TNode *zchild_root = z->child;
            TNode *zchild = zchild_root;
            do {
                TNode *move = zchild; zchild = zchild->l;
                AddToList(Min, move); move->p=nullptr;
            } while (zchild != zchild_root);
            z->child=nullptr; z->degree=0;
        }
        if (z == z->r) {
            Min = nullptr;
        } else {
            Min = z->r;
            z->l->r = z->r;
            z->r->l = z->l;
            Consolidate();
            z->l=z->r=z;
        }
        --N; return z;
    }
    void Union(TFibonacciMinHeap& other) {
        if (other.Min == nullptr) return;
        if (Min == nullptr) { Min = other.Min; return; }
        TNode *x = other.Min;
        TNode *y = x->l;
        TNode *a = Min->l;
        TNode *b = Min;
        x->l = a; a->r = x;
        y->r = b; b->l = y;
        if (x->key < b->key) Min = x;
        N += other.N;
        other.Min = nullptr;
        other.N = 0;
    }
    void DecreaseKey(TNode *x, i32 key) {
        if (key > x->key) return; // error
        x->key = key;
        TNode *y = x->p;
        if (y != nullptr && x->key < y->key) {
            Cut(x, y);
            CascadingCut(y);
        }
        if (x->key < Min->key) Min = x;
    }

    void Delete(TNode *x) {
        DecreaseKey(x, INT32_MIN);
        if (x != ExtractMin()) {
            std::cout << "Delete error\n";
        }
    }

    void DoPrint(u32 indent, TNode *min) const noexcept {
        if (min == nullptr) { std::cout << "[empty]\n"; return; }
        std::cout << "\n";

        TNode *curr=min;
        do{
            u32 i=indent; while(i--) std::cout << ' ';
            std::cout << "[" << curr->key
                      << ": mark=" << curr->mark
                      << ", degree=" << curr->degree
                      << ", left=" << curr->l->key
                      << ", right=" << curr->r->key;
            if (curr->p) std::cout << ", parent=" << curr->p->key;
            if (curr->child) {
                std::cout << ", child=";
                DoPrint(indent+2, curr->child);
            }
            std::cout << "]\n";

            curr=curr->l;
        }while(curr != min);
    }

    void DebugPrint() const noexcept {
        std::cout << "Fibonacci heap (N=" << N << ", Min is the first line below):";
        DoPrint(0, Min);
    }
};


#ifndef RUN_TESTS
#include "cache64.hpp"
#include "printer.hpp"
void run_tests() {
    struct TStore : TCache64<TFibonacciNode<u32>, 32> {
        using TNode = TFibonacciNode<u32>;
        TNode *MakeNode(i32 key) {
            TNode *x = Allocate();
            x->key = key;
            return x;
        }
    } s;
    TFibonacciMinHeap<u32> fh;

    auto _23 = s.MakeNode(23);
    fh.Insert(_23);
    fh.Insert(s.MakeNode(7));
    // fh.DebugPrint();

    TFibonacciMinHeap<u32> fh2;
    fh2.Insert(s.MakeNode(21));
    fh2.Insert(s.MakeNode(3));
    // fh2.DebugPrint();

    fh.Union(fh2);
    fh.DebugPrint();
    // fh2.DebugPrint();

    DEBUG(fh.ExtractMin()->key);
    fh.DecreaseKey(_23, 1);
    fh.DebugPrint();
}
#endif
