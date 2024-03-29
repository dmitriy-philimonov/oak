#include <iostream>
#include <vector>


#include "fibonacci_heap.hpp"


class TDijkstra {
    static constexpr u32 MAX=std::numeric_limits<u32>::max();

private:
    struct TEdge {
        u32 V;
        u32 W;
    };
    std::vector<std::vector<TEdge>> AdjL;

public:
    // number of vertexes from 0 to VertexCount-1
    TDijkstra(u32 VertexCount)
        : AdjL(VertexCount)
    {}

    void AddEdge(u32 u, u32 v, u32 w) {
        AdjL[u].push_back({v,w});
    }

    std::vector<u32> ShortestPath(u32 from) {
        std::vector<u32> D(AdjL.size(), MAX);
        TFibonacciMinHeapGraph fh(AdjL.size());

        D[from] = 0; fh.Decrease(from, 0); fh.DebugPrint();
        while(!fh.Empty()) {
            u32 u = fh.Top().V; fh.Pop();
            std::cout << "TOP:" << u << '\n';
            for(TEdge edge: AdjL[u]) {
                u32 newD = D[u] + edge.W;
                if (newD < D[edge.V]) {
                    D[edge.V] = newD;
                    fh.Decrease(edge.V, newD);
                    fh.DebugPrint();
                }
            }
        }
        return D;
    }
};


void PrintD(const std::vector<uint32_t>& D) {
    using ui32 = uint32_t;
    std::cout << "D:{";
    for(ui32 v=0; v<D.size(); ++v) {
        std::cout << v << '/';
        if (D[v] == std::numeric_limits<ui32>::max()) std::cout << "MAX";
        else std::cout << D[v];
        std::cout << ',';
    }
    std::cout << "\b}\n";
}


int main() {
    TDijkstra dij(5);
    dij.AddEdge(0,3,5);
    dij.AddEdge(0,1,10);

    dij.AddEdge(1,3,2);
    dij.AddEdge(1,2,1);

    dij.AddEdge(2,4,4);

    dij.AddEdge(3,2,9);
    dij.AddEdge(3,4,2);
    dij.AddEdge(3,1,3);

    dij.AddEdge(4,2,6);
    dij.AddEdge(4,0,7);

    PrintD(dij.ShortestPath(0));
    return 0;
}
