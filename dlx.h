// dlx solver interface
// Bill Ola Rasmussen
#include "Solver.h"
#include <vector>
#include <iostream>

struct Node {
	Node*L,*R,*U,*D,*C; // left, right, up, down, head
	Node():L(this),R(this),U(this),D(this),C(this){std::cout<<"Node "<<this<<"\n";}
	virtual ~Node(){std::cout<<"~Node "<<this<<"\n";}
	Node*Linkup(Node*rhs);
};
struct HeadNode : public Node {
	int S,N; // size, name
	HeadNode(int name):S(0),N(name){}
	~HeadNode(){std::cout<<"~HeadNode "<<this<<"\n";}
};
struct RaiiNodes {
	std::vector<Node*>v;
	~RaiiNodes(){
	    std::cout<<"cleanup"<<std::endl;
	    for(auto p:v){delete p;}
	}
};

class DLX : public Solver {
	RaiiNodes n;
public:
	DLX(int pc, int sc);
	virtual ~DLX();
	void Row();
	void Col(int column);
	void Solve();
};

