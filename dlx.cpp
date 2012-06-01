// classic dancing links solver implementation
// Bill Ola Rasmussen
#include "dlx.h"

// todo: cleanup
#include <assert.h>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
using namespace std;

// note: comments marked with '|' indicate text from the paper
// http://www-cs-faculty.stanford.edu/~uno/papers/dancing-color.ps.gz

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

// Algorithm X dlx data structure
// ------------------------------
// | One good way to implement algorithm X is to represent each 1 in the
// | matrix A as a data object x with five fields L[x], R[x], U[x], D[x], C[x]. Rows of the matrix
// | are doubly linked as circular lists via the L and R fields (“left” and “right”); columns are
// | doubly linked as circular lists via the U and D fields (“up” and “down”). Each column
// | list also includes a special data object called its list header.
// |     The list headers are part of a larger object called a column object. Each column object
// | y contains the fields L[y], R[y], U [y], D[y], and C[y] of a data object and two additional
// | fields, S[y] (“size”) and N [y] (“name”); the size is the number of 1s in the column, and the
// | name is a symbolic identifier for printing the answers. The C field of each object points
// | to the column object at the head of the relevant column.
// |     The L and R fields of the list headers link together all columns that still need to be
// | covered. This circular list also includes a special column object called the root, h, which
// | serves as a master header for all the active headers. The fields U[h], D[h], C[h], S[h], and
// | N[h] are not used.

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
void DLX::Solve()
{
    cout<<"Solve with "<<n.v.size()<<" nodes\n";
    HeadNode* h{n.GetHead(-1)};
    cout<<"head nodes: ";
    for(Node* p{h->R};p!=h;p=p->R)
    {
        cout<<((HeadNode*)p)->N<<" ";
    }
    cout<<'\n';
    
}
