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

// note: comments marked with '|' indicate text from the paper
// http://www-cs-faculty.stanford.edu/~uno/papers/dancing-color.ps.gz

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
	auto ph(dynamic_cast<HeadNode*>(v[col+1].get()));
	if(!ph){throw(runtime_error("failed GetHead, constraint index may be out of range"));}
	if(col!=ph->N){throw(runtime_error("bad Head node name"));}
	return ph;
}

vector<std::unique_ptr<Node>>RaiiNodes::Snap()
{
	vector<unique_ptr<Node>> x;
	for(const auto&i:v)
	{
		x.push_back(i.get()->Clone());
	}
	return x;
}

//#include <typeinfo>
bool RaiiNodes::Comp(vector<std::unique_ptr<Node>>&x)
{
	if(x.size()!=v.size()){return false;}
	for(size_t i(0);i<x.size();++i)
	{
		//cout<<"types: "<<typeid(*x[i].get()).name()<<" "<<typeid(*v[i].get()).name()<<"\n";
		if(!x[i].get()->Same(v[i].get()))
		{
			return false;
		}
	}
	return true;
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

void DLX::Init(const int pc, const int sc, std::function<void(Event)>CallBack)
{
	Notify=CallBack;
	
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
void DLX::Row(const int col)
{
	n.V(rowStart=n.GetHead(col)->LinkU(new Node()));
}
void DLX::Col(const int col)
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

void DLX::Solve(const bool showSoln)
{
	show=showSoln;
	
	cout<<"Solve with "<<n.Size()<<" nodes\n";
	
	vector<unique_ptr<Node>>x(n.Snap()); // capture start state
	if(!n.Comp(x)){throw(runtime_error("early node structure integrity failure"));}
	
	vector<Node*>O;
	Notify(Event::Begin);
	Search(n.GetHead(-1),0,O);
	Notify(Event::End);

	if(!n.Comp(x)){throw(runtime_error("node structure integrity failure"));}
	cout<<"Node structure integrity verified.\n";
}

// Algorithm Details
// -----------------
// | Our nondeterministic algorithm to find all exact covers can now be cast in the following
// | explicit, deterministic form as a recursive procedure search(k), which is invoked initially
// | with k = 0:
// |     If R[h] = h, print the current solution (see below) and return.
// |     Otherwise choose a column object c (see below).
// |     Cover column c (see below).
// |     For each r ← D[c], D[D[c]] , ..., while r ≠ c,
// |         set O sub k ← r;
// |         for each j ← R[r], R[R[r]] , ..., while j ≠ r,
// |             cover column j (see below);
// |         search(k + 1);
// |         set r ← O sub k and c ← C[r];      * note 1
// |         for each j ← L[r], L[L[r]] , ..., while j ≠ r,
// |             uncover column j (see below).
// |     Uncover column c (see below) and return.
//
// Note 1: r and c already have these values if recursion is used.
// This hint might mean that a non recursive implementation is possible
// if we keep r stacked on O.

void DLX::Search(HeadNode*h,int k,vector<Node*>&O)
{
	if(h==h->R) // no head nodes
	{
		Notify(Event::Soln);
		if(show){ShowSolution(k,O);}
		return;
	}

	HeadNode*c=ChooseColumn(h);
	if(!c)
	{
		// discover that there is no way to cover a column in this
		// search branch, return
		// | If column c is entirely zero, there are no subalgorithms
		// | and the process terminates unsuccessfully.
		// cout<<"unable to cover column with remaining rows\n";
		return;
	}
	
	//cout<<"Choose column "<<c->N<<" with count "<<c->S<<" level "<<k<<'\n';
	Cover(c);
	for(Node*r=c->D;r!=c;r=r->D) // all the rows in column c
	{
		O.push_back(r); // implements: set O sub k ← r;
		for(Node*j=r->R;j!=r;j=j->R) // all the nodes in row
		{
			Cover(j->C);
		}        

		Search(h,k+1,O);

		O.pop_back(); // implements: set r ← O sub k and c ← C[r];
		for(Node*j=r->L;j!=r;j=j->L) // all the nodes in row
		{
			Uncover(j->C);
		}        
	}
	Uncover(c);
}

// | The operation of printing the current solution is easy: We successively print the rows
// | containing O sub 0, O sub 1, ..., O sub k−1 , where the row containing data object O is
// | printed by printing N[C[O]], N[C[R[O]]], N[C[R[R[O]]]], etc.

void DLX::ShowSolution(int /*k*/,std::vector<Node*>&O)
{
	cout<<"[\n";
	
	for(const auto& r:O)
	{
		cout<<r->C->N<<" ";
		for(Node*j=r->R;j!=r;j=j->R) // all the nodes in row
		{
			cout<<j->C->N<<" ";
		}
		cout<<"\n";
	}
	
	cout<<"]\n";
}

// Minimize Search Branching Factor
// --------------------------------
// | To choose a column object c, we could simply set c ← R[h]; this is the leftmost
// | uncovered column. Or if we want to minimize the branching factor, we could set s ← ∞
// | and then:
// |     for each j ← R[h], R[R[h]], ..., while j ≠ h,
// |         if S[j] < s set c ← j and s ← S[j].
// | Then c is a column with the smallest number of 1s. (The S fields are not needed unless
// | we want to minimize branching in this way.)
//
// Side note: special case for queens, Knuth points out that things go faster
// if a central locations are chosen first. In the queens case, each primary constraint
// column has the same number of child nodes, so this implementation could be
// improved as we always start with the first column.

HeadNode*DLX::ChooseColumn(HeadNode*h) // least covered column
{
	// todo: implement as described, without optimization
	// in: at least one column head node on list
	HeadNode*j=static_cast<HeadNode*>(h->R); // init first as min
	if(!j->S){return nullptr;} // early return, no way to cover a column
	for(HeadNode*p=static_cast<HeadNode*>(j->R);p!=h;p=static_cast<HeadNode*>(p->R))
	{
		if(!p->S){return nullptr;} // early return, no way to cover a column
		if(p->S<j->S){j=p;}
	}
	return j;
}

// Covering a Column
// -----------------
// | The operation of covering column c is more interesting: It removes c from the header
// | list and removes all rows in c’s own list from the other column lists they are in.
// |     Set L[R[c]] ← L[c] and R[L[c]] ← R[c].
// |     For each i ← D[c], D[D[c]] , ..., while i ≠ c,
// |         for each j ← R[i], R[R[i]] , ..., while j ≠ i,
// |             set U[D[j]] ← U [j], D[U[j]] ← D[j],
// |             and set S[C[j]] ← S[C[j]] − 1.

void DLX::Cover(HeadNode*c)
{
	//cout<<"Covering column "<<c->N<<" with count "<<c->S<<'\n';
	// remove self from head node list
	c->R->L=c->L;
	c->L->R=c->R;
	// process column
	for(Node*i=c->D;i!=c;i=i->D) // all rows having nodes in this column
	{
		for(Node*j=i->R;j!=i;j=j->R) // all _other_ nodes in this row
		{
			// remove node from column
			j->D->U=j->U;
			j->U->D=j->D;
			// inform column head that it has one less node
			--(j->C->S);
		}
	}
}

// Uncovering a Column
// -------------------
// | Here is where the links do their dance:
// |     For each i = U[c], U[U[c]] , ..., while i ≠ c,
// |         for each j ← L[i], L[L[i]] , ..., while j ≠ i,
// |             set S[C[j]] ← S[C[j]] + 1,
// |             and set U[D[j]] ← j, D[U[[j]] ← j.
// |     Set L[R[c]] ← c and R[L[c]] ← c.

void DLX::Uncover(HeadNode*c)
{
	//cout<<"Uncovering column "<<c->N<<" with count "<<c->S<<'\n';
	// operations carried out in reverse order of Cover()

	// process column
	for(Node*i=c->U;i!=c;i=i->U) // all rows having nodes in this column, reverse order
	{
		for(Node*j=i->L;j!=i;j=j->L) // all _other_ nodes in this row, reverse order
		{
			// inform column head that its node came back
			++(j->C->S);
			// insert node back into column
			j->D->U=j;
			j->U->D=j;
		}
	}
	// reinsert self into head node list
	c->R->L=c;
	c->L->R=c;
}

// consider:
// - rework data structure so static_casts are not needed
