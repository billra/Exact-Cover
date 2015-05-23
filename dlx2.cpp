// modified dancing links solver implementation
// Bill Ola Rasmussen

#include "dlx2.h"
#include <stdexcept>
#include <memory>
#include <iostream>
using namespace std;

Node2*Node2::LinkL(Node2*p) // place node in same row before this item
{
	p->R=this;
	p->L=L;
	L->R=p;
	L=p;
	return p;
}
Node2*HeadNode2::LinkU(Node2*p) // place node in same column before this item
{
	p->C=this;
	++S;
	p->D=this;
	p->U=U;
	U->D=p;
	U=p;
	return p;    
}
HeadNode2*RaiiNodes2::GetHead(int col) // col is -1 for header head
{
	// verification overhead, do not use at solve time
	if((int)v.size()-1<col+1){throw(runtime_error("GetHead index out of range"));}
	auto ph(dynamic_cast<HeadNode2*>(v[col+1].get()));
	if(!ph){throw(runtime_error("failed HeadNode2 dynamic cast"));}
	if(col!=ph->N){throw(runtime_error("bad Head node name"));}
	return ph;
}

vector<std::unique_ptr<Node2>>RaiiNodes2::Snap()const
{
	vector<unique_ptr<Node2>> x;
	for(const auto&i:v)
	{
		x.emplace_back(i.get()->Clone());
	}
	return x;
}

//#include <typeinfo>
bool RaiiNodes2::Comp(vector<std::unique_ptr<Node2>>&x)const
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

void DLX2::Init(const int pc, const int sc)
{
	n.V(new HeadNode2(-1)); // head node of head nodes
	for(int i(0);i<pc;++i) // primary constraint head nodes
	{
		n.V(n.GetHead(-1)->LinkL(new HeadNode2(i))); // link on raw pointers
	}
	for(int i(0);i<sc;++i) // secondary constraint head nodes
	{
		n.V(new HeadNode2(i+pc)); // no link to peers
	}
}
void DLX2::Row(const int col)
{
	n.V(rowStart=n.GetHead(col)->LinkU(new Node2()));
}
void DLX2::Col(const int col)
{    
	n.V(rowStart->LinkL(n.GetHead(col)->LinkU(new Node2())));
}

void DLX2::Solve(const bool showSoln, std::function<void(Event)>CallBack)
{
	show=showSoln;
	Notify = CallBack;

	cout<<"DLX2::Solve with "<<n.Size()<<" nodes\n";
	
	vector<unique_ptr<Node2>>x(n.Snap()); // capture start state
	if(!n.Comp(x)){throw(runtime_error("early node structure integrity failure"));}
	
	vector<Node2*>O;
	Notify(Event::Begin);
	Search(n.GetHead(-1),0,O);
	Notify(Event::End);

	if(!n.Comp(x)){throw(runtime_error("node structure integrity failure"));}
	cout<<"Node2 structure integrity verified.\n";
}

void DLX2::Search(HeadNode2*h,int k,vector<Node2*>&O)
{
	if(h==h->R) // no head nodes
	{
		Notify(Event::Soln);
		if(show){ShowSolution(k,O);}
		return;
	}

	HeadNode2*c=ChooseColumn(h);
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
	for(Node2*r=c->D;r!=c;r=r->D) // all the rows in column c
	{
		O.emplace_back(r); // implements: set O sub k ← r;
		for(Node2*j=r->R;j!=r;j=j->R) // all the nodes in row
		{
			Cover(j->C);
		}        

		Search(h,k+1,O);

		O.pop_back(); // implements: set r ← O sub k and c ← C[r];
		for(Node2*j=r->L;j!=r;j=j->L) // all the nodes in row
		{
			Uncover(j->C);
		}        
	}
	Uncover(c);
}

void DLX2::ShowSolution(int /*k*/,std::vector<Node2*>&O)const
{
	cout<<"[\n";
	
	for(const auto& r:O)
	{
		cout<<r->C->N<<" ";
		for(Node2*j=r->R;j!=r;j=j->R) // all the nodes in row
		{
			cout<<j->C->N<<" ";
		}
		cout<<"\n";
	}
	
	cout<<"]\n";
}


HeadNode2*DLX2::ChooseColumn(HeadNode2*h)const // least covered column
{
	// todo: implement as described, without optimization
	// in: at least one column head node on list
	HeadNode2*j=static_cast<HeadNode2*>(h->R); // init first as min
	if(!j->S){return nullptr;} // early return, no way to cover a column
	for(HeadNode2*p=static_cast<HeadNode2*>(j->R);p!=h;p=static_cast<HeadNode2*>(p->R))
	{
		if(!p->S){return nullptr;} // early return, no way to cover a column
		if(p->S<j->S){j=p;}
	}
	return j;
}

void DLX2::Cover(HeadNode2*c)
{
	//cout<<"Covering column "<<c->N<<" with count "<<c->S<<'\n';
	// remove self from head node list
	c->R->L=c->L;
	c->L->R=c->R;
	// process column
	for(Node2*i=c->D;i!=c;i=i->D) // all rows having nodes in this column
	{
		for(Node2*j=i->R;j!=i;j=j->R) // all _other_ nodes in this row
		{
			// remove node from column
			j->D->U=j->U;
			j->U->D=j->D;
			// inform column head that it has one less node
			--(j->C->S);
		}
	}
}

void DLX2::Uncover(HeadNode2*c)
{
	//cout<<"Uncovering column "<<c->N<<" with count "<<c->S<<'\n';
	// operations carried out in reverse order of Cover()

	// process column
	for(Node2*i=c->U;i!=c;i=i->U) // all rows having nodes in this column, reverse order
	{
		for(Node2*j=i->L;j!=i;j=j->L) // all _other_ nodes in this row, reverse order
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
