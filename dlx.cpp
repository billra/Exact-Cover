// classic dancing links solver implementation
// Bill Ola Rasmussen
#include "dlx.h"

// todo: cleanup
#include <assert.h>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <limits>
using namespace std;

Node*Node::LinkL(Node*p) // place node in same row before this item
{
    p->R=this;
    p->L=this->L;
    this->L->R=p;
    this->L=p;
    return p;
}
Node*HeadNode::LinkU(Node*p) // place node in same column before this item
{
    p->C=this;
    ++S;
    p->D=this;
    p->U=this->U;
    this->U->D=p;
    this->U=p;
    return p;    
}
HeadNode*RaiiNodes::GetHead(int col) // col is -1 for header head
{
    // verification overhead, avoid using this at solve time
    auto ph=dynamic_cast<HeadNode*>(v[col+1]);
    if(!ph){throw(runtime_error("failed GetHead, constraint index may be out of range"));}
    if(col!=ph->N){throw(runtime_error("bad Head node name"));}
    return ph;
}

// Generalized Exact Cover
// -----------------------
// | We can handle this extra complication by generalizing the exact cover problem. Instead
// | of requiring all columns of a given 0-1 matrix to be covered by disjoint rows, we
// | will distinguish two kinds of columns: primary and secondary. The generalized problem
// | asks for a set of rows that covers every primary column exactly once and every secondary
// | column at most once.
// 
// | Fortunately, we can solve the generalized cover problem by using almost the same
// | algorithm as before. The only difference is that we initialize the data structure by making
// | a circular list of the column headers for the primary columns only. The header for each
// | secondary column should have L and R fields that simply point to itself. The remainder
// | of the algorithm proceeds exactly as before, so we will still call it algorithm DLX.
//
// see insertion of secondary constraint head nodes below

void DLX::Init(int pc, int sc)
{
    n.V(new HeadNode(-1)); // head node of head nodes
    for(int i(0);i<pc;++i) // primary constraint head nodes
    {
        n.V(n.GetHead(-1)->LinkL(new HeadNode(i))); // link on raw pointers
    }
    for(int i(0);i<sc;++i) // secondary constraint head nodes
    {
        n.V(new HeadNode(i+pc)); // no link to peers
    }
}
void DLX::Row(int col)
{
    n.V(rowStart=n.GetHead(col)->LinkU(new Node()));
}
void DLX::Col(int col)
{    
    n.V(rowStart->LinkL(n.GetHead(col)->LinkU(new Node())));
}

// Knuth's Algorithm X
// -------------------
// | If A is empty, the problem is solved; terminate successfully.
// | Otherwise choose a column, c (deterministically).
// | Choose a row, r, such that A[r,c] = 1 (nondeterministically).
// | Include r in the partial solution.
// | For each j such that A[r,j] = 1,
// |     delete column j from matrix A;
// |         for each i such that A[i,j] = 1,
// |             delete row i from matrix A.
// | Repeat this algorithm recursively on the reduced matrix A.
//
// deterministically: choose column with fewest 1's

void DLX::Solve()
{
    cout<<"Solve with "<<n.Size()<<" nodes\n";
    Search(n.GetHead(-1));
}

// Our nondeterministic algorithm to find all exact covers can now be cast in the following
// explicit, deterministic form as a recursive procedure search(k), which is invoked initially
// with k = 0:
//     If R[h] = h, print the current solution (see below) and return.
//     Otherwise choose a column object c (see below).
//     Cover column c (see below).
//     For each r ← D[c], D D[c] , . . . , while r = c,
//         set Ok ← r;
//         for each j ← R[r], R R[r] , . . . , while j = r,
//             cover column j (see below);
//         search(k + 1);
//         set r ← Ok and c ← C[r];
//         for each j ← L[r], L L[r] , . . . , while j = r,
//             uncover column j (see below).
//     Uncover column c (see below) and return.


void DLX::Search(HeadNode*hh)
{
    if(hh==hh->R) // no head nodes
    {
        cout<<"Solution...\n";
        // todo: show solution
        return;
    }

    HeadNode*c{ChooseColumn(hh)};
    if(!c)
    {
        cout<<"unable to cover column with remaining rows\n";
        return;
    }
    
    cout<<"Choose column "<<c->N<<" with count "<<c->S<<'\n';

    Cover(c);
    
    
    // todo


}


// Minimize Search Branching Factor
// --------------------------------
// | To choose a column object c, we could simply set c ← R[h]; this is the leftmost
// | uncovered column. Or if we want to minimize the branching factor, we could set s ← ∞
// | and then:
// |     for each j ← R[h], R R[h] , ..., while j = h,
// |         if S[j] < s set c ← j and s ← S[j].
// | Then c is a column with the smallest number of 1s. (The S fields are not needed unless
// | we want to minimize branching in this way.)
//
// Side note: special case for queens, Knuth points out that things go faster
// if a central location is chosen. In the queens case, each primary constraint
// column has the same number of child nodes, so this implementation could be
// improved as we always start with the first column.

HeadNode*DLX::ChooseColumn(HeadNode*hh) // least covered column
{
    // in: at least one column head node on list
    HeadNode*pMin{static_cast<HeadNode*>(hh->R)}; // init first as min
    if(!pMin->S){return nullptr;} // early return, no way to cover a column
    for(HeadNode*p{static_cast<HeadNode*>(pMin->R)};p!=hh;p=static_cast<HeadNode*>(p->R))
    {
        if(!p->S){return nullptr;} // early return, no way to cover a column
        if(p->S<pMin->S){pMin=p;}
    }
    return pMin;
}

// Covering a Column
// -----------------
// | The operation of covering column c is more interesting: It removes c from the header
// | list and removes all rows in c’s own list from the other column lists they are in.
// |     Set L R[c] ← L[c] and R L[c] ← R[c].
// |     For each i ← D[c], D D[c] , ..., while i = c,
// |         for each j ← R[i], R R[i] , ..., while j = i,
// |             set U D[j] ← U [j], D U [j] ← D[j],
// |             and set S C[j] ← S C[j] − 1.

void DLX::Cover(HeadNode*h)
{
    cout<<"Covering column "<<h->N<<" with count "<<h->S<<'\n';

}

// consider:
// - at solve time, loops might be simplified if last R is null
// - rework data structure so static_casts are not needed
