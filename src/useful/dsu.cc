#include <vector>
#include <stdint.h>


class TDSU{
    using u32=uint32_t;
    struct T{
        u32 Parent;
        u32 Rank;
    };
    std::vector<T> Tree;
public:
    TDSU(u32 n)
        : Tree(n)
    {
        for(u32 i=0;i<n;++i){
            Tree[i]={i,0};
        }
    }

    u32 Find(u32 x) noexcept {
        T& curr = Tree[x];
        if(curr.Parent == x) return x;
        return curr.Parent = Find(curr.Parent);
    }

    void Union(u32 x, u32 y) noexcept {
        x = Find(x); y = Find(y);
        T &X = Tree[x], &Y=Tree[y]; 
        if (X.Rank > Y.Rank) {
            Y.Parent = x; return;
        } else {
            /* X.Rank <= Y.Rank */
            X.Parent = y;
            if (X.Rank == Y.Rank) ++Y.Rank;
        }
    }

    u32 CountUniq() const noexcept {
        u32 cnt=0;
        for(u32 i=0; i<Tree.size(); ++i){
            cnt += Tree[i].Parent == i;
        }
        return cnt;
    }
};


int main() {
    return 0;
}