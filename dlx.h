// dlx solver interface
// Bill Ola Rasmussen
#include "Solver.h"
#include <vector>
#include <iostream>

struct Node {
    Node*L,*R,*U,*D,*C; // left, right, up, down, head
    Node():L(this),R(this),U(this),D(this),C(this){std::cout<<"Node "<<this<<"\n";}
    virtual ~Node(){std::cout<<"~Node "<<this<<"\n";}
    Node*LinkL(Node*p);
};

struct HeadNode : public Node {
    int S,N; // size, name
    HeadNode(int name):S(0),N(name){std::cout<<"Head Node "<<this<<"\n";}
    virtual ~HeadNode(){std::cout<<"~HeadNode "<<this<<"\n";}
    Node*LinkU(Node*p);
};

class RaiiNodes {
public: // todo
    std::vector<Node*>v;
    ~RaiiNodes(){
        std::cout<<"cleanup"<<std::endl;
        for(auto p:v){delete p;}
    }
    void V(Node*p){v.push_back(p);}
    HeadNode*GetHead(int col); // col is -1 for header head
};

class DLX:public Solver{
    RaiiNodes n; // allocation bucket
    Node*rowStart;
public:
    DLX(){} // should not need to define this?
    void Init(int pc, int sc);
    void Row(int col);
    void Col(int col);
    void Solve();
    DLX&operator=(const DLX&)=delete; // no assignment
    DLX(const DLX&)=delete; // no copy constructor
};

