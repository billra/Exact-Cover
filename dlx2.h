// dlx2 solver interface
// Bill Ola Rasmussen
#include "Solver.h"
#include <vector>
#include <memory>

class HeadNode2;
class Node2 {
public:
	Node2*L,*R,*U,*D; // left, right, up, down
	HeadNode2*C; // head
	Node2():L(this),R(this),U(this),D(this),C(nullptr){}
	virtual ~Node2(){}
	Node2*LinkL(Node2*p);
	// node integrity test support
	virtual std::unique_ptr<Node2>Clone()const{return std::unique_ptr<Node2>(new Node2(*this));}
	virtual bool Same(const Node2*const n)const
	{
		return L==n->L&&R==n->R&&U==n->U&&D==n->D&&C==n->C;
	}
};

class HeadNode2 : public Node2 {
public:
	int S,N; // size, name
	HeadNode2(int name):Node2(),S(0),N(name){}
	HeadNode2():Node2(),S(0){}
	Node2*LinkU(Node2*p);
	// node integrity test support
	std::unique_ptr<Node2>Clone()const override{return std::unique_ptr<HeadNode2>(new HeadNode2(*this));}
	bool Same(const Node2*const n)const override// must have same signature, so casting later on...
	{
		const auto h(dynamic_cast<const HeadNode2*const>(n));
		return h&&Node2::Same(n)&&S==h->S&&N==h->N;
	}
};

// The implementation of the algorithm uses raw pointers. Node2 lifetime
// is managed by unique_ptr vector.

struct RaiiNodes2 {
	std::vector<std::unique_ptr<Node2>>v;
	std::vector<HeadNode2>vh; // contiguous vector of all head nodes, no heap allocation
	void V(Node2*p) { v.emplace_back(std::unique_ptr<Node2>(p)); }
	HeadNode2*GetHead(int col); // col is -1 for header head, aka. hh
	// node integrity test support
	std::vector<std::unique_ptr<Node2>> Snap()const;
	bool Comp(std::vector<std::unique_ptr<Node2>>&x)const;
};

class DLX2:public Solver{
	RaiiNodes2 n; // allocation bucket
	Node2*rowStart; // used for inserting rows and columns when reading 
	std::function<void(Event)>Notify;
	bool show;
	void Search(HeadNode2*const hh,std::vector<Node2*>&O);
	void ShowSolution(const std::vector<Node2*>&O)const;
	HeadNode2*ChooseColumn(HeadNode2*const hh)const;
	void Cover(HeadNode2*const c);
	void Uncover(HeadNode2*const c);
public:
	DLX2(){} // should not need to define this?
	void Init(const int pc, const int sc) override;
	void Row(const int col) override;
	void Col(const int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	DLX2&operator=(const DLX2&)=delete; // no assignment
	DLX2(const DLX2&)=delete; // no copy constructor
};

// make hh a member and don't pass