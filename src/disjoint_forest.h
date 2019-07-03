#pragma once

/*
 * "Introduction to Algorithms, third edition" by
 *     Thomas H. Cormen
 *     Charles E. Leiserson
 *     Ronald L. Rivest
 *     Clifford Stein
 *
 * Chapter 21.3 Disjoint set forests (page 568)
 *
 * Heuristics to improve the running time:
 *     -> union by rank
 *     -> path compression
 */

#include <iostream>
#include <deque>
#include <vector>

namespace NDisjointSetForest {

    class TTree {
    public:
        struct TDisjointSetInfo {
            unsigned Rank;
            unsigned NumberOfMembers;
            TDisjointSetInfo()
                : Rank(0)
                , NumberOfMembers(1)
            {}
            // for DebugPrint only
            operator bool () const noexcept {
                return NumberOfMembers != 0;
            }
            // for DebugPrint only
            void Free() noexcept {
                NumberOfMembers = 0;
            }
        };
        struct TNode {
            TNode* Parent;
            TDisjointSetInfo Info;
            unsigned Data;

            TNode()
                : Parent(nullptr)
                , Data(0)
            {}

            void Activate(unsigned data) noexcept {
                Parent = this;
                Data = data;
            }

            operator bool () const noexcept {
                return Parent != nullptr;
            }

            void DebugPrint() const noexcept {
                std::cout << "{Data=" << Data << ",Parent=" << Parent->Data;
                if (Info) {
                    std::cout << ",Rank=" << Info.Rank << ",NumberOfMembers=" << Info.NumberOfMembers;
                }
                std::cout << "}\n";
            }
        };

    private:
        /* O(alpha(n)) --> O(1), because alpha(n) < 4 for any feasible n */
        TNode* FindSetImpl(TNode* node) {
            // quick exist for root
            if (node == node->Parent)
                return node;

            std::vector<TNode*> todo;
            // is root?
            while (node != node->Parent) {
                todo.push_back(node); // no, remember
                node = node->Parent;
            }
            // set root
            TNode* root = node;
            // path comprehension
            while(!todo.empty()) {
                todo.back()->Parent = root;
                todo.pop_back();
            }

            return root;
        }
        /* O(1) */
        void Link(TNode* x, TNode* y) {
            // already the same set
            if (x == y)
                return;

            if (x->Info.Rank > y->Info.Rank) {
                y->Parent = x;

                x->Info.NumberOfMembers += y->Info.NumberOfMembers;
                y->Info.Free();
            } else {
                x->Parent = y;

                if (x->Info.Rank == y->Info.Rank)
                    ++y->Info.Rank;
                y->Info.NumberOfMembers += x->Info.NumberOfMembers;
                x->Info.Free();
            }
        }

    private:
        std::deque<TNode> StorageOfNodes;
        unsigned MaxElement = 0;
        TNode* RegisterNewNode(const unsigned member) {
            if (member >= StorageOfNodes.size())
                StorageOfNodes.resize(StorageOfNodes.size() * 2);
            MaxElement = std::max(MaxElement, member); // all time MaxElement < StorageOfNodes.size()

            auto& storage = StorageOfNodes[member];
            storage.Activate(member);
            return &storage;
        }

    public:
        TTree(const unsigned hint)
            : StorageOfNodes(hint)
        {}

        TTree()
            : TTree(16)
        {}

        /* O(1) */
        void MakeSet(unsigned x) {
            RegisterNewNode(x);
        }

        /* O(alpha(n)) */
        void Union(unsigned x, unsigned y) {
            TNode* Sx = FindSet(x);
            TNode* Sy = FindSet(y);
            if (Sx && Sy)
                Link(Sx, Sy);
            else
                std::cerr << "Union(): incorrect input " << x << ", " << y << std::endl;
        }

        /* O(alpha(n)) --> O(1) */
        TNode* FindSet(unsigned x) {
            if (x <= MaxElement) {
                if (TNode& curr = StorageOfNodes[x])
                    return FindSetImpl(&curr);
            }
            return nullptr;
        }

        void DebugPrint(bool verbose = true) const noexcept {
            auto CalcHeight = [] (const TNode* node) -> unsigned {
                unsigned height = 0;
                while(node != node->Parent) {
                    ++height;
                    node = node->Parent;
                }
                return height;
            };

            std::cout << " === DisjoinstSetForest === \n";
            std::deque<unsigned> HeightStats;
            unsigned realElements = 0;
            unsigned maxRank = 0;
            unsigned maxMembers = 0;
            for(unsigned idx=0; idx<=MaxElement; ++idx) {
                auto& node = StorageOfNodes[idx];
                if (node) {
                    if (verbose)
                        node.DebugPrint();
                    ++realElements;
                    maxRank = std::max(maxRank, node.Info.Rank);
                    maxMembers = std::max(maxMembers, node.Info.NumberOfMembers);

                    unsigned currHeight = CalcHeight(&node);
                    if (currHeight >= HeightStats.size())
                        HeightStats.resize(HeightStats.size() + 1);
                    ++HeightStats[currHeight];
                }
            }

            for(unsigned height=0; height < HeightStats.size(); ++height) {
                std::cout << "HEIGHT: " << height << " OCCURS " << HeightStats[height] << "\n";
            }
            std::cout << "NUM ELEMENTS: " << realElements << " (RESERVED: " << StorageOfNodes.size() << ")\n"
                    << "MAX RANK: " << maxRank << "\n"
                    << "MAX MEMBERS: " << maxMembers << "\n"
                    << "APPROXIMATE MEMORY CONSUMED: " << sizeof(TNode) * StorageOfNodes.size() / 1024 << "K\n";
        }
    };

} // NDisjointSetForest