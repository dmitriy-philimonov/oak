#include "rbset.h"

#include <iostream>
#include <vector>


namespace NRBTree {
    // definitions for static objects
    TTree::TNode TTree::TNode::NilObj = TTree::TNode(TTree::TNode::TForNilObj());
    TTree::TNode* TTree::Nil = &TTree::TNode::NilObj;

    void TTree::DoLeftRotate(TNode* x) {
        TNode* y = x->Right;
        // x and y must exist

        x->Right = y->Left;
        if (y->Left != Nil)
            y->Left->Parent = x;
        
        TNode* xParent = x->Parent;
        y->Left = x;
        x->Parent = y;
        
        y->Parent = xParent;
        if (xParent == Nil) {
            Root = y;
            return;
        }
        if (x == xParent->Left) {
            xParent->Left = y;
            return;
        }
        xParent->Right = y;
    }
    void TTree::DoRightRotate(TNode* y) {
        TNode* x = y->Left;
        // x and y must exist

        y->Left = x->Right;
        if (x->Right != Nil)
            x->Right->Parent = y;
        
        TNode* yParent = y->Parent;
        x->Right = y;
        y->Parent = x;
        
        x->Parent = yParent;
        if (yParent == Nil) {
            Root = x;
            return;
        }
        if (y == yParent->Left) {
            yParent->Left = x;
            return;
        }
        yParent->Right = x;
    }

    void TTree::LeftRotate(TNode* x) {
        DoLeftRotate(x);
        x->Parent->Size = x->Size; // y.size = x.size
        x->Size = x->Left->Size + x->Right->Size + 1;
    }
    void TTree::RightRotate(TNode* y) {
        DoRightRotate(y);
        y->Parent->Size = y->Size; // x.size = y.size
        y->Size = y->Left->Size + y->Right->Size + 1;
    }
    void TTree::RbInsertFixup(TNode* z) {
        while (z->Parent->Color == RED) {
            if (z->Parent == z->Parent->Parent->Left) {
                TNode* y = z->Parent->Parent->Right;
                if (y->Color == RED) {
                    z->Parent->Color = BLACK;
                    y->Color = BLACK;
                    z->Parent->Parent->Color = RED;
                    z = z->Parent->Parent;
                    continue;
                } // else
                if (z == z->Parent->Right) {
                    z = z->Parent;
                    LeftRotate(z);
                }
                z->Parent->Color = BLACK;
                z->Parent->Parent->Color = RED;
                RightRotate(z->Parent->Parent);
                continue;
            } // else
            TNode* y = z->Parent->Parent->Left;
            if (y->Color == RED) {
                z->Parent->Color = BLACK;
                y->Color = BLACK;
                z->Parent->Parent->Color = RED;
                z = z->Parent->Parent;
                continue;
            } // else
            if (z == z->Parent->Left) {
                z = z->Parent;
                RightRotate(z);
            }
            z->Parent->Color = BLACK;
            z->Parent->Parent->Color = RED;
            LeftRotate(z->Parent->Parent);
        }
        Root->Color = BLACK;
    }
    void TTree::RbDeleteFixup(TNode* x) {
        while(x != Root && x->Color == BLACK) {
            if (x == x->Parent->Left) {
                TNode* w = x->Parent->Right;
                if (w->Color == RED) {
                    w->Color = BLACK; // Case 1
                    x->Parent->Color = RED;
                    LeftRotate(x->Parent);
                    w = x->Parent->Right;
                }
                if (w->Left->Color == BLACK && w->Right->Color == BLACK) {
                    w->Color = RED;   // Case 2
                    x = x->Parent;
                    continue;
                }
                if (w->Right->Color == BLACK) {
                    w->Left->Color = BLACK; // Case 3
                    w->Color = RED;
                    RightRotate(w);
                    w = x->Parent->Right;
                }
                w->Color = x->Parent->Color;// Case 4
                x->Parent->Color = BLACK;
                w->Right->Color = BLACK;
                LeftRotate(x->Parent);
                break;
            } // else
            // x == x->Parent->Right
            TNode* w = x->Parent->Left;
            if (w->Color == RED) {
                w->Color = BLACK; // Case 1
                x->Parent->Color = RED;
                RightRotate(x->Parent);
                w = x->Parent->Left;
            }
            if (w->Left->Color == BLACK && w->Right->Color == BLACK) {
                w->Color = RED;   // Case 2
                x = x->Parent;
                continue;
            }
            if (w->Left->Color == BLACK) {
                w->Right->Color = BLACK; // Case 3
                w->Color = RED;
                LeftRotate(w);
                w = x->Parent->Left;
            }
            w->Color = x->Parent->Color; // Case 4
            x->Parent->Color = BLACK;
            w->Left->Color = BLACK;
            RightRotate(x->Parent);
            break;
        }
        x->Color = BLACK;
    }
    TTree::TNode* TTree::Successor(TNode* x) noexcept {
        if (x->Right != Nil) return Minimum(x->Right);
        TNode* y = x->Parent;
        while(y != Nil && y->Right == x) {
            x = y;
            y = y->Parent;
        }
        return y;
    }
    TTree::TNode* TTree::Predecessor(TNode* x) noexcept {
        if (x->Left != Nil) return Maximum(x->Left);
        TNode* y = x->Parent;
        while(y != Nil && y->Left == x) {
            x = y;
            y = y->Parent;
        }
        return y;
    }
    TTree::TNode* TTree::Select(ui32 i, TNode* x) const noexcept {
        if (i == 0 || i > Root->Size)
            return Nil;

        while(true) {
            ui32 r = x->Left->Size + 1;
            if (i == r) return x;
            if (i < r) {
                x = x->Left;
                continue;
            }
            i -= r;
            x = x->Right;
        }
    }
    ui32 TTree::Rank(TNode* x) noexcept {
        if (x == Nil) return 0;
        
        ui32 r = x->Left->Size + 1;
        while(x->Parent != Nil) {
            if (x == x->Parent->Right)
                r += x->Parent->Left->Size + 1;
            x = x->Parent;
        }
        return r;
    }

    TTree::TNode* TTree::DeleteNode(TNode* z) noexcept {
        if (z == Nil) return Nil;

        auto Transplant = [this](TNode* u, TNode* v) {
            if (v != Nil)
                v->Parent = u->Parent;
            if (u->Parent == Nil) {
                Root = v;
                return;
            }
            if (u == u->Parent->Left) {
                u->Parent->Left = v;
                return;
            }
            u->Parent->Right = v;
        };
        auto FixSize = [this](TNode* x) {
            while(x != Nil) {
                --x->Size;
                x = x->Parent;
            }
        };
        auto rbDeleteFixup = [this](TNode* p, TNode*& pLeaf, TNode* x, bool yoc) {
            // p == Nil if deleted node was Root
            if (p == Nil || yoc == RED) return;
            if (x != Nil) return RbDeleteFixup(x);
            // thread safe emulation
            TNode dummyX = TNode(TNode::TForNilObj()); pLeaf = &dummyX; dummyX.Parent = p;
            RbDeleteFixup(&dummyX);
            pLeaf = x; // in all 4 cases dummyX stays the same leaf of p (left or right)
        };
        auto rbDeleteFixupEasy = [this](TNode* x, bool yoc) {
            // x is guaranted to be a real node
            if (yoc == RED) return;
            RbDeleteFixup(x);
        };

        TNode* y = z;
        TNode* x = Nil;
        bool y_original_color = y->Color;

        if (z->Left == Nil) {
            x = z->Right;
            Transplant(z, z->Right); FixSize(z->Parent);
            return rbDeleteFixup(z->Parent, z->Parent->Right, x, y_original_color), z;
        }
        if (z->Right == Nil) {
            x = z->Left;
            Transplant(z, z->Left); FixSize(z->Parent);
            return rbDeleteFixupEasy(x, y_original_color), z;
        }
        y = Minimum(z->Right);
        y_original_color = y->Color;
        x = y->Right;
        FixSize(y->Parent);
        TNode* x_new_parent = y;
        bool x_is_left_child = false;
        if (y->Parent != z) {
            Transplant(y, y->Right);
            y->Right = z->Right;
            y->Right->Parent = y;
            x_new_parent = y->Parent;
            x_is_left_child = true;
        }
        Transplant(z, y);
        y->Left = z->Left;
        y->Left->Parent = y;
        y->Color = z->Color;
        y->Size = y->Left->Size + y->Right->Size + 1;
        if (x_is_left_child)
            return rbDeleteFixup(x_new_parent, x_new_parent->Left, x, y_original_color), z;
        return rbDeleteFixup(x_new_parent, x_new_parent->Right, x, y_original_color), z;
    }

    void TTree::DebugInfo() const noexcept {
        if (Root == Nil) return;
        if (Root->Color != BLACK) {
            std::cout << "Violation of rule 2: root is always black\n";
            return;
        }
        struct H {
            TNode* Node;
            ui32 Height;
            ui32 BlackHeight;
        };
        ui32 maxHeight=0;
        ui32 maxBlackHeight=0;
        std::vector<H> todo; todo.push_back({Root, 1, 1});
        while(!todo.empty()) {
            H h = todo.back(); todo.pop_back();
            TNode* cur = h.Node;
            maxHeight = std::max(maxHeight, h.Height);
            if (cur->Right != Nil)
                todo.push_back(
                    {cur->Right, h.Height+1, h.BlackHeight + (cur->Right->Color == BLACK)}
                );
            if (cur->Left != Nil)
                todo.push_back(
                    {cur->Left, h.Height+1, h.BlackHeight + (cur->Left->Color == BLACK)}
                );
            if (cur->Left == Nil && cur->Right == Nil) {
                if (maxBlackHeight == 0) {
                    maxBlackHeight = h.BlackHeight;
                    continue;
                }
                if (h.BlackHeight != maxBlackHeight) {
                    std::cout << "Violation of rule 5: for each node all simple paths form "
                    "the node to descendant leaves contain the same number of lack nodes" << '\n';
                }
            }
        }
        // black height includes Nil, which is always black
        std::cout << "Height=" << maxHeight << " BlackHeight=" << maxBlackHeight + 1 << '\n';
    }
}
