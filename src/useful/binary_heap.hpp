#include <iostream>
#include <vector>


class TMinHeapGraph {
    using u32 = uint32_t;
    static constexpr u32 EMPTY=u32(-1);
    static constexpr u32 MAX=u32(-1);
private:
    struct TDV {
        u32 D; // distance
        u32 V; // vertex
    };
    std::vector<TDV> Q;   // queue
    std::vector<u32> V2Q; // vertex to index in queue

    inline u32 Left(u32 i) const noexcept {
        return 2*i+1;
    }
    inline u32 Right(u32 i) const noexcept {
        return 2*i+2;
    }
    inline u32 Parent(u32 i) const noexcept {
        return (i-1)/2;
    }
    void DoDecrease(u32 i) {
        while(i>0) {
            u32 p = Parent(i);
            if (Q[p].D <= Q[i].D) break;
            std::swap(V2Q[Q[p].V], V2Q[Q[i].V]);
            std::swap(Q[p], Q[i]);
            i=p;
        }
    }
    void MinHeapify(u32 i) {
        while(true) {
            u32 l = Left(i);
            if (l >= Q.size()) break;

            u32 minimal = i;
            if (Q[l].D < Q[minimal].D)
                minimal = l;

            u32 r = Right(i);
            if (r < Q.size() && Q[r].D < Q[minimal].D)
                minimal = r;

            if (minimal == i) break;
            std::swap(V2Q[Q[i].V], V2Q[Q[minimal].V]);
            std::swap(Q[i], Q[minimal]);
            i = minimal;
        }
    }
    void Push(u32 d, u32 v) {
        u32 qi = Q.size(); Q.push_back({d,v});
        V2Q[v] = qi;
        DoDecrease(qi);
    }
public:
    // supports vertexes from 0 .. vertex_count-1
    // programming model = all vertexes have MAX distance initially
    TMinHeapGraph(u32 vertex_count)
        : Q(vertex_count, {MAX, 0})
        , V2Q(vertex_count, EMPTY)
    {
        for(u32 v=0; v<vertex_count; ++v) {
            Q[v].V=v; V2Q[v]=v;
        }
    }
    void Decrease(u32 v, u32 d) noexcept {
        if (V2Q[v] == EMPTY) return;

        u32 qi = V2Q[v];
        if (Q[qi].D <= d) return;

        Q[qi].D = d;
        DoDecrease(qi);
    }
    TDV Top() const noexcept { return Q.front(); }
    bool Empty() const noexcept { return Q.empty(); }
    bool Has(u32 v) const noexcept { return V2Q[v] != EMPTY; }
    void Pop() {
        V2Q[Q.front().V] = EMPTY; if (Q.size()==1) { Q.clear(); return; }
        V2Q[Q.back().V] = 0;

        Q.front() = Q.back(); Q.pop_back(); MinHeapify(0);
    }

    void DebugPrint() const noexcept {
        std::cout << "Q={";
        for(u32 i=0; i<Q.size(); ++i) {
            if (Q[i].D==MAX) std::cout << "MAX"; else std::cout << Q[i].D;
            std::cout << '/' << Q[i].V << ',';
        }
        std::cout << "\b}\n";

        std::cout << "V2Q={";
        for(u32 i=0; i<V2Q.size(); ++i) {
            if (V2Q[i] == EMPTY) std::cout << "EMPTY,";
            else std::cout << V2Q[i] << ',';
        }
        std::cout << "\b}\n";
    }
};