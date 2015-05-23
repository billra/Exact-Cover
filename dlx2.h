// dlx solver interface
// Bill Ola Rasmussen
#include "Solver.h"
#include <vector>
#include <iostream>
#include <memory>

// note: comments marked with '|' indicate text from the paper
// http://www-cs-faculty.stanford.edu/~uno/papers/dancing-color.ps.gz

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

class HeadNode2;
class Node2 {
public:
	Node2*L,*R,*U,*D; // left, right, up, down
	HeadNode2*C; // head
	Node2():L(this),R(this),U(this),D(this),C(nullptr){/*std::cout<<"Node2 "<<this<<"\n";*/}
	virtual ~Node2(){/*std::cout<<"~Node2 "<<this<<"\n";*/}
	Node2*LinkL(Node2*p);
	// node integrity test support
	virtual std::unique_ptr<Node2>Clone()const{return std::unique_ptr<Node2>(new Node2(*this));}
	virtual bool Same(const Node2*const n)const
	{
		//std::cout<<" node comp\n";
		return L==n->L&&R==n->R&&U==n->U&&D==n->D&&C==n->C;
	}
};

class HeadNode2 : public Node2 {
public:
	int S,N; // size, name
	HeadNode2(int name):S(0),N(name){/*std::cout<<"Head Node2 "<<this<<"\n";*/}
	virtual ~HeadNode2(){/*std::cout<<"~HeadNode2 "<<this<<"\n";*/}
	Node2*LinkU(Node2*p);
	// node integrity test support
	std::unique_ptr<Node2>Clone()const override{return std::unique_ptr<HeadNode2>(new HeadNode2(*this));}
	bool Same(const Node2*const n)const override// must have same signature, so casting later on...
	{
		//std::cout<<" HeadNode2 comp";
		const auto h(dynamic_cast<const HeadNode2*const>(n));
		return h&&Node2::Same(n)&&S==h->S&&N==h->N;
	}
};

// The implementation of the algorithm uses raw pointers. Node2 lifetime
// is managed by unique_ptr vector.

class RaiiNodes2 {
	std::vector<std::unique_ptr<Node2>>v;
public:
	void V(Node2*p){v.emplace_back(std::unique_ptr<Node2>(p));}
	HeadNode2*GetHead(int col); // col is -1 for header head
	size_t Size()const{return v.size();}
	// node integrity test support
	std::vector<std::unique_ptr<Node2>> Snap()const;
	bool Comp(std::vector<std::unique_ptr<Node2>>&x)const;
};

class DLX2:public Solver{
	RaiiNodes2 n; // allocation bucket
	Node2*rowStart;
	std::function<void(Event)>Notify;
	bool show;
	void Search(HeadNode2*h,int k,std::vector<Node2*>&O);
	void ShowSolution(int k,std::vector<Node2*>&O)const;
	HeadNode2*ChooseColumn(HeadNode2*hh)const;
	void Cover(HeadNode2*c);
	void Uncover(HeadNode2*c);
public:
	DLX2(){} // should not need to define this?
	void Init(const int pc, const int sc) override;
	void Row(const int col) override;
	void Col(const int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	DLX2&operator=(const DLX2&)=delete; // no assignment
	DLX2(const DLX2&)=delete; // no copy constructor
};

