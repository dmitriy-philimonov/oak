#pragma once

/*
 * Augmented Red Black Tree (set/map) - based on famous CLRS book)
 *  1. Supports order statistics (order is 1-based: from 1 to N): select/rank
 *  2. Max 2G objects (size and color is packed in ui32, so size has ui31 type)
 *  3. STL-like iterators supported (begin/rbegin/find/select)
 * 
 * P.S. multi_* versions aren't supported
 */

#include "defines.h"
#include <iostream>
#include <vector>

namespace NRBTree {
    struct TDefaultAllocator {
        static void* Allocate(size_t size) {
            return ::operator new(size);
        }
        static void DeAllocate(void* ptr) noexcept {
            return ::operator delete(ptr);
        }
    };

    template<typename T, typename TAllocator>
    struct TConstructor {
        template<typename ...Args>
        static T* Construct(Args&&... args) {
            void* ptr = TAllocator::Allocate(sizeof(T));
            return new (ptr) T(std::forward<Args>(args)...);
        }
        static void Destruct(T* obj) noexcept {
            obj->~T();
            TAllocator::DeAllocate(obj);
        }
    };
    

    class TTree {
    public:
        enum TColor {
            BLACK = 0,
            RED = 1
        };

        struct TNode;
        static TNode* Nil;

        struct TNode {
            TNode* Left   = Nil;
            TNode* Right  = Nil;
            TNode* Parent = Nil;
            ui32 Color:1, Size:31;

            TNode()
                : Color(RED)
                , Size(1U)
            {}
            struct TForNilObj {};
            TNode(TForNilObj)
                : Color(BLACK)
                , Size(0U)
            {}
            static TNode NilObj;
        };

    private:
        void DoLeftRotate(TNode* x);
        void DoRightRotate(TNode* y);
        void LeftRotate(TNode* x);
        void RightRotate(TNode* y);

    protected:
        TNode* Root = Nil;
    protected:
        void RbInsertFixup(TNode* z);
        void RbDeleteFixup(TNode* x);
        TNode* DeleteNode(TNode* z) noexcept;
        
        static TNode* Minimum(TNode* x) noexcept {
            while(x->Left != Nil) x = x->Left;
            return x;
        }
        static TNode* Maximum(TNode* x) noexcept {
            while(x->Right != Nil) x = x->Right;
            return x;
        }
        static TNode* Successor(TNode* x) noexcept;
        static TNode* Predecessor(TNode* x) noexcept;

        /* rank from 1 to N, where N = Size() */
        TNode* Select(ui32 i, TNode* x) const noexcept;
        static ui32 Rank(TNode* x) noexcept;
        ui32 Size(TNode* x) const noexcept { return x->Size; }
    protected:
        template<void(*Destroy)(TNode*)>
        void ClearImpl() noexcept {
            TNode* x = Root;
            while(x != Nil) {
                if (x->Left != Nil) {
                    x = x->Left;
                    continue;
                }
                if (x->Right != Nil) {
                    x = x->Right;
                    continue;
                }
                TNode* d = x; x = x->Parent;
                if (d == x->Left) x->Left = Nil; else x->Right = Nil;
                Destroy(d);
            }
            Root = x;
        }
    public:
        ui32 size() const noexcept { return Size(Root); }
        void DebugInfo() const noexcept;
        friend class TIteratorBase;
        friend class TIterator;
        friend class TReverseIterator;
    };

    class TIteratorBase {
    public:
        using TNode = TTree::TNode;
    protected:
        TNode* Cur = TTree::Nil;
    public:
        TIteratorBase(TNode* cur): Cur(cur) {}
        bool operator == (const TIteratorBase& other) const noexcept {
            return Cur == other.Cur;
        }
        bool operator != (const TIteratorBase& other) const noexcept {
            return Cur != other.Cur;
        }
        /* rank is a number between 1 and N, where N = Size(Root), 0 indicates error */
        ui32 rank() const noexcept {
            return TTree::Rank(Cur);
        }
        TTree::TColor color() const noexcept {
            return static_cast<TTree::TColor>(Cur->Color);
        }
    };

    class TIterator: public TIteratorBase {
    public:
        TIterator(TNode* cur): TIteratorBase(cur) {}
        TIterator& operator ++() noexcept {
            Cur = TTree::Successor(Cur);
            return *this;
        }
        TIterator& operator ++(int) noexcept {
            return this->operator++();
        }
        TIterator& operator --() noexcept {
            Cur = TTree::Predecessor(Cur);
            return *this;
        }
        TIterator& operator --(int) noexcept {
            return this->operator--();
        }
    };
    class TReverseIterator: public TIteratorBase {
    public:
        TReverseIterator(TNode* cur): TIteratorBase(cur) {}
        TReverseIterator& operator ++() noexcept {
            Cur = TTree::Predecessor(Cur);
            return *this;
        }
        TReverseIterator& operator ++(int) noexcept {
            return this->operator++();
        }
        TReverseIterator& operator --() noexcept {
            Cur = TTree::Successor(Cur);
            return *this;
        }
        TReverseIterator& operator --(int) noexcept {
            return this->operator--();
        }
    };

    template<typename TKey, typename TDerivedNode, typename TAllocator>
    class TTreeInsDel : public TTree {
    private:
        using TTree::Nil;

        /* virtual functions emulation,
         * I don't want to pay extra sizeof(void*) for vtable in TNode
         */
        static inline const TKey& GetKey(TNode* x) noexcept {
            return static_cast<TDerivedNode*>(x)->Key;
        }
        static inline void PrintMe(TNode* x) noexcept {
            static_cast<TDerivedNode*>(x)->PrintMe();
        }
        static inline void DestroyNode(TNode* x) noexcept {
            TConstructor<TDerivedNode, TAllocator>::Destruct(static_cast<TDerivedNode*>(x));
        }
    public:
        void clear() noexcept { TTree::ClearImpl<DestroyNode>(); }
    protected:
        template<typename ...Args>
        bool InsertImpl(const TKey& key, Args&&... args) noexcept {
            TNode* y = Nil;
            TNode* x = Root;
            while(x != Nil) {
                y = x; ++x->Size;
                if (key < GetKey(x)) {
                    x = x->Left;
                    continue;
                }
                if (GetKey(x) < key) {
                    x = x->Right;
                    continue;
                }
                /* key == GetKey(x) => fix already altered sizes and return */
                do { --x->Size; x = x->Parent; } while(x != Nil);
                return false;
            }
            TNode* z = TConstructor<TDerivedNode, TAllocator>::Construct(key, std::forward<Args>(args)...);
            z->Parent = y;
            if (y == Nil) {
                Root = z;
                return RbInsertFixup(z), true;
            }
            if (GetKey(z) < GetKey(y)) y->Left = z; else y->Right = z;
            return RbInsertFixup(z), true;
        }
        TNode* Search(const TKey& key) const noexcept {
            TNode* x = Root;
            while(x != Nil) {
                if (key < GetKey(x)) { x = x->Left; continue; }
                if (GetKey(x) < key) { x = x->Right; continue; }
                /* key == GetKey(x) */
                return x;
            }
            return x;
        }
    public:
        bool exists(const TKey& key) const noexcept {
            return Search(key) != Nil;
        }
        /* rank is a number between 1 and N, where N = Size(Root), 0 indicates error */
        ui32 rank(const TKey& key) const noexcept {
            TNode* z = Search(key);
            if (z == Nil) return 0;
            return TTree::Rank(z);
        }
        /* returns true if success, otherwise returns false (the key hasn't found) */
        bool erase(const TKey& key) noexcept {
            TNode* z = DeleteNode(Search(key));
            if (z == Nil) return false;
            DestroyNode(z); return true;
        }

    public:
        void DebugPrint() const noexcept;
        void IncreaseOrder(std::vector<TKey>& keys) const noexcept {
            keys.reserve(size());
            for(TNode* cur = Minimum(Root); cur != Nil; cur = Successor(cur))
                keys.push_back(GetKey(cur));
        }
        void DecreaseOrder(std::vector<TKey>& keys) const noexcept {
            keys.reserve(size());
            for(TNode* cur = Maximum(Root); cur != Nil; cur = Predecessor(cur))
                keys.push_back(GetKey(cur));
        }
        void PreOrder(std::vector<TKey>& keys, TNode* x = Nil) const noexcept {
            if (Root == Nil) return;
            if (x == Nil) return PreOrder(keys, Root);
            keys.push_back(GetKey(x));
            if (x->Left != Nil) PreOrder(keys, x->Left);
            if (x->Right != Nil) PreOrder(keys, x->Right);
        }
        void InOrder(std::vector<TKey>& keys, TNode* x = Nil) const noexcept {
            if (Root == Nil) return;
            if (x == Nil) return InOrder(keys, Root);
            if (x->Left != Nil) InOrder(keys, x->Left);
            keys.push_back(GetKey(x));
            if (x->Right != Nil) InOrder(keys, x->Right);
        }
        void PostOrder(std::vector<TKey>& keys, TNode* x = Nil) const noexcept {
            if (Root == Nil) return;
            if (x == Nil) return PostOrder(keys, Root);
            if (x->Left != Nil) PostOrder(keys, x->Left);
            if (x->Right != Nil) PostOrder(keys, x->Right);
            keys.push_back(GetKey(x));
        }
    };

    template<typename TKey, typename TData>
    struct TKeyData {
        const TKey Key;
        TData Data;
        TKeyData(const TKey& k)
            : Key(k)
            , Data()
        {}
        template<typename ...Args>
        TKeyData(const TKey& k, Args&&... args)
            : Key(k)
            , Data(std::forward<Args>(args)...)
        {}
    };

    template<typename TKey>
    struct TNodeKey : public TTree::TNode {
        const TKey Key;
        TNodeKey(const TKey& k)
            : Key(k)
        {}
        void PrintMe() const noexcept {
            auto GetColor = [] (bool c) {
                return (c) ? 'R' : 'B';
            };
            std::cout << "{K=" << Key << ",S=" << Size << ",C=" << GetColor(Color) << '}';
        }
    };

    template<typename TKey, typename TData>
    struct TNodeData : public TTree::TNode, public TKeyData<TKey, TData> {
        using TBase = TKeyData<TKey, TData>;
        TNodeData(const TKey& key)
            : TBase(key)
        {}
        template<typename ...Args>
        TNodeData(const TKey key, Args&&...args)
            : TBase(key, std::forward<Args>(args)...)
        {}
        void PrintMe() const noexcept {
            auto GetColor = [] (bool c) {
                return (c) ? 'R' : 'B';
            };
            std::cout << "{K=" << TBase::Key << ",D=" << TBase::Data << ",S=" << Size << ",C=" << GetColor(Color) << '}';
        }
    };

    template<typename TKey, typename TAllocator = TDefaultAllocator>
    class TSet : public TTreeInsDel<TKey, TNodeKey<TKey>, TAllocator> {
        using TNode = TTree::TNode;
    public:
        ~TSet() { this->clear(); }
    public:
        /* returns true if success, otherwise returns false (the key already exists) */
        bool insert(const TKey& key) {
            return this->template InsertImpl(key);
        }
        void insert(std::initializer_list<TKey> range) {
            for(auto it=range.begin(); it!=range.end(); ++it)
                insert(*it);
        }
    public:
        class TSetIterator : public TIterator {
        public:
            TSetIterator(TNode* cur): TIterator(cur) {}
            const TKey* operator ->() const noexcept {
                return &this->operator*();
            }
            const TKey& operator *()  const noexcept {
                return static_cast<TNodeKey<TKey>*>(Cur)->Key;
            }
        };
        class TReverseSetIterator: public TReverseIterator {
        public:
            TReverseSetIterator(TNode* cur): TReverseIterator(cur) {}
            const TKey* operator ->() const noexcept {
                return &this->operator*();
            }
            const TKey& operator *()  const noexcept {
                return static_cast<TNodeKey<TKey>*>(Cur)->Key;
            }
        };
        TSetIterator find(const TKey& key) const noexcept {
            return TSetIterator(this->Search(key));
        }
        TSetIterator begin() const noexcept {
            return TSetIterator(TTree::Minimum(TTree::Root));
        }
        TSetIterator end() const noexcept {
            return TSetIterator(TTree::Nil);
        }
        TSetIterator select(ui32 rank) const noexcept {
            return TSetIterator(TTree::Select(rank, TTree::Root));
        }
        TReverseIterator rbegin() const noexcept {
            return TReverseSetIterator(TTree::Maximum(TTree::Root));
        }
        TReverseIterator rend() const noexcept {
            return TReverseSetIterator(TTree::Nil);
        }
    };

    template<typename TKey, typename TData, typename TAllocator = TDefaultAllocator>
    class TMap : public TTreeInsDel<TKey, TNodeData<TKey, TData>, TAllocator> {
        using TNode = TTree::TNode;
    public:
        ~TMap() { this->clear(); }
    public:
        /* returns true if success, otherwise returns false (the key already exists) */
        bool insert(const TKey& key, const TData& data) noexcept {
            return this->template InsertImpl(key, data);
        }
        template<typename ...Args>
        bool emplace(const TKey& key, Args&&... args) noexcept {
            return this->template InsertImpl(key, std::forward<Args>(args)...);
        }
    public:
        class TMapIterator : public TIterator {           
        public:
            TMapIterator(TNode* cur): TIterator(cur) {}            

            const TKeyData<TKey, TData>* operator ->() const noexcept {
                return static_cast<TNodeData<TKey, TData>*>(Cur);
            }
            const TKeyData<TKey, TData>& operator *()  const noexcept {
                return *this->operator->();
            }
        };
        class TReverseMapIterator : public TReverseIterator {
        public:
            TReverseMapIterator(TNode* cur): TReverseIterator(cur) {}
            const TKeyData<TKey, TData>* operator ->() const noexcept {
                return static_cast<TNodeData<TKey, TData>*>(Cur);
            }
            const TKeyData<TKey, TData>& operator *()  const noexcept {
                return *this->operator->();
            }
        };
        TMapIterator find(const TKey& key) const noexcept {
            return TMapIterator(this->Search(key));
        }
        TMapIterator select(ui32 rank) const noexcept {
            return TMapIterator(TTree::Select(rank, TTree::Root));
        }
        TMapIterator begin() const noexcept {
            return TMapIterator(TTree::Minimum(TTree::Root));
        }
        TMapIterator end() const noexcept {
            return TMapIterator(TTree::Nil);
        }
        TReverseMapIterator rbegin() const noexcept {
            return TReverseMapIterator(TTree::Maximum(TTree::Root));
        }
        TReverseMapIterator rend() const noexcept {
            return TReverseMapIterator(TTree::Nil);
        }
    };

    template<typename TKey, typename TDerivedNode, typename TAllocator>
    void TTreeInsDel<TKey, TDerivedNode, TAllocator>::DebugPrint() const noexcept {
        /* print out our NilObj */
        std::cout << "Nil=" << (void*)Nil << '(' << sizeof(*Nil) << "b){"
        << "L=" << (void*)Nil->Left << ','
        << "R=" << (void*)Nil->Right << ','
        << "P=" << (void*)Nil->Parent << ','
        << "S=" << Nil->Size << ','
        << "C=" << ((Nil->Color) ? 'R' : 'B') << '}'
        << '\n';
        /* print out our tree */
        if (Root == Nil) { std::cout << "Graph={}\n"; return; }
        
        std::cout << "Graph={\n";
        struct TQ{
            TNode* Node;
            ui32 Level;
        };
        std::vector<TQ> todo(1, {Root, 0U});
        while(!todo.empty()) {
            TQ q = todo.back(); todo.pop_back();
            TNode* cur = q.Node;
            ui32 l = q.Level; while(l--) std::cout << '-';
            PrintMe(cur); std::cout << '\n';
            if (cur->Right != Nil) todo.push_back({cur->Right, q.Level+1});
            if (cur->Left != Nil) todo.push_back({cur->Left, q.Level+1});
        }
        std::cout << "}\n";
    }
}
