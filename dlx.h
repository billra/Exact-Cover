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

class HeadNode;
class Node {
public:
	Node*L,*R,*U,*D; // left, right, up, down
	HeadNode*C; // head
	Node():L(this),R(this),U(this),D(this),C(nullptr){/*std::cout<<"Node "<<this<<"\n";*/}
	virtual ~Node(){/*std::cout<<"~Node "<<this<<"\n";*/}
	Node*LinkL(Node*p);
	// node integrity test support
	virtual std::unique_ptr<Node>Clone()const{return std::unique_ptr<Node>(new Node(*this));}
	virtual bool Same(const Node*const n)const
	{
		//std::cout<<" node comp\n";
		return L==n->L&&R==n->R&&U==n->U&&D==n->D&&C==n->C;
	}
};

class HeadNode : public Node {
public:
	int S,N; // size, name
	HeadNode(int name):S(0),N(name){/*std::cout<<"Head Node "<<this<<"\n";*/}
	virtual ~HeadNode(){/*std::cout<<"~HeadNode "<<this<<"\n";*/}
	Node*LinkU(Node*p);
	// node integrity test support
	virtual std::unique_ptr<Node>Clone()const{return std::unique_ptr<HeadNode>(new HeadNode(*this));}
	virtual bool Same(const Node*const n)const // must have same signature, so casting later on...
	{
		//std::cout<<" HeadNode comp";
		auto h=dynamic_cast<const HeadNode*const>(n);
		return h&&Node::Same(n)&&S==h->S&&N==h->N;
	}
};

// The implementation of the algorithm uses raw pointers. Node lifetime
// is managed by unique_ptr vector.

class RaiiNodes {
	std::vector<std::unique_ptr<Node>>v;
public:
	void V(Node*p){v.push_back(std::unique_ptr<Node>(p));}
	HeadNode*GetHead(int col); // col is -1 for header head
	size_t Size(){return v.size();}
	// node integrity test support
	std::vector<std::unique_ptr<Node>> Snap();
	bool Comp(std::vector<std::unique_ptr<Node>>&x);
};

class DLX:public Solver{
	RaiiNodes n; // allocation bucket
	Node*rowStart;
	void Search(HeadNode*h,int k,std::vector<Node*>&O);
	void ShowSolution(int k,std::vector<Node*>&O);
	HeadNode*ChooseColumn(HeadNode*hh);
	void Cover(HeadNode*c);
	void Uncover(HeadNode*c);
	void(*Notify)(Event);
	bool show;
public:
	DLX(){} // should not need to define this?
	void Init(const int pc, const int sc, void(*CallBack)(Event)) override;
	void Row(const int col) override;
	void Col(const int col) override;
	void Solve(const bool showSoln=true) override;
	DLX&operator=(const DLX&)=delete; // no assignment
	DLX(const DLX&)=delete; // no copy constructor
};

