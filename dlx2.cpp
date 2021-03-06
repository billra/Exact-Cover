// modified dancing links solver implementation
// Bill Ola Rasmussen

#include "dlx2.h"
#include <stdexcept>
#include <memory>
#include <iostream>
using namespace std;

Node2*Node2::LinkL(Node2*p) // place node in same row before this item
{
	p->R=this; p->L=L;
	L->R=p;	L=p;
	return p;
}
Node2*HeadNode2::LinkU(Node2*p) // place node in same column before this item
{
	++S;
	p->C = this;
	p->D=this; p->U=U;
	U->D=p;	U=p;
	return p;    
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

bool RaiiNodes2::Comp(vector<std::unique_ptr<Node2>>&x)const
{
	if(x.size()!=v.size()){return false;}
	for(size_t i(0);i<x.size();++i)
	{
		if(!x[i].get()->Same(v[i].get()))
		{
			return false;
		}
	}
	return true;
}

HeadNode2*RaiiNodes2::GetHead(int col) // col is -1 for header head
{
	// verification overhead, do not use at solve time
	if ((int)vh.size() - 1<col + 1) { 
		throw(runtime_error("GetHead index out of range")); 
	}
	HeadNode2* ph(&vh[col + 1]);
	if (col != ph->N) { 
		throw(runtime_error("bad Head node name")); 
	}
	return ph;
}

// contiguous head array to increase locality of reference
void DLX2::Init(const unsigned int pc, const unsigned int sc)
{
	n.vh.resize(1 + pc + sc); // head node of head nodes, primary constraints, secondary constraints
	for (vector<HeadNode2>::size_type i(0); i < n.vh.size(); ++i) // all nodes get a name, hh == -1
	{
		n.vh[i].N = i - 1;
	}
	for (unsigned int i(1); i < pc; ++i) // circular link primary constraint nodes
	{
		n.GetHead(-1)->LinkL(&n.vh[i]);
	}
}
// 2. todo: make contiguous nodes in tiles
void DLX2::Row(const unsigned int col)
{
	n.V(rowStart=n.GetHead(col)->LinkU(new Node2()));
}
void DLX2::Col(const unsigned int col)
{    
	n.V(rowStart->LinkL(n.GetHead(col)->LinkU(new Node2())));
}

void DLX2::Solve(const bool showSoln, std::function<void(Event)>CallBack)
{
	show=showSoln;
	Notify = CallBack;

	cout<<"DLX2::Solve with "<<n.vh.size()-1<<" head nodes, " << n.v.size() << " nodes\n";
	
	vector<unique_ptr<Node2>>x(n.Snap()); // capture start state
	if(!n.Comp(x)){throw(runtime_error("early node structure integrity failure"));}
	
	vector<Node2*>Soln;
	Notify(Event::Begin);
	Search(n.GetHead(-1),Soln);
	Notify(Event::End);

	if(!n.Comp(x)){throw(runtime_error("node structure integrity failure"));}
	cout<<"Node2 structure integrity verified.\n";
}

// optimization:	remove recursion from search
// result:			no improvement
// conclusion:		in below case, compiler does at least as good a job as I can do by hand

void DLX2::Search(HeadNode2* const hh,vector<Node2*>&Soln)
{
	// because of goto recursion replacement, it is necessary to have a dummy value which is never used
	HeadNode2*c = hh;

	vector<Node2*>rStack(n.v.size(),nullptr); // fixed preallocated buffer, space overkill using count of all nodes
	vector<Node2*>::size_type irStack(0); // index of unused position (i.e. rStack.end() )

recurse:

	if(hh==hh->R) // no head nodes
	{
		Notify(Event::Soln);
		if(show){ShowSolution(Soln);}
		goto pop; // was: return;
	}

	c=ChooseColumn(hh);
	if(!c)
	{
		// discover that there is no way to cover a column in this
		// search branch, return
		goto pop; // was: return;
	}
	
	Cover(c);
	for(Node2*r=c->D;r!=c;r=r->D) // all the rows in column c
	{
		Soln.emplace_back(r); // implements: set Soln sub k ← r;
		for(Node2*j=r->R;j!=r;j=j->R) // all the nodes in row
		{
			Cover(j->C);
		}        

		// replace recursion: Search(hh,k+1,Soln);
		rStack[irStack++] = r;
		goto recurse;
	pop:
		if (!irStack) { return; } // stack is fully unwound
		r = rStack[--irStack];
		c = r->C;

		Soln.pop_back();
		for(Node2*j=r->L;j!=r;j=j->L) // all the nodes in row
		{
			Uncover(j->C);
		}        
	}
	Uncover(c);
	goto pop; // was: default return
}

void DLX2::Cover(HeadNode2*const c)
{
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

void DLX2::Uncover(HeadNode2*const c)
{
	// operations carried out in reverse order of Cover()

	// process column
	for(Node2*i=c->U;i!=c;i=i->U) // all rows having nodes in this column, reverse order
	{
		for (Node2*j = i->R; j != i; j = j->R) // all _other_ nodes in this row (reverse order actually not necessary!)
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

HeadNode2*DLX2::ChooseColumn(HeadNode2*const hh)const // least covered column
{
	// todo: implement as described, without optimization
	// in: at least one column head node on list
	HeadNode2*j = static_cast<HeadNode2*>(hh->R); // initialize first as min
	if (!j->S) { return nullptr; } // early return, no way to cover a column
	for (HeadNode2*p = static_cast<HeadNode2*>(j->R); p != hh; p = static_cast<HeadNode2*>(p->R))
	{
		if (!p->S) { return nullptr; } // early return, no way to cover a column
		if (p->S<j->S) { j = p; }
	}
	return j;
}

void DLX2::ShowSolution(const std::vector<Node2*>&O)const
{
	cout << "[\n";

	for (const auto& r : O)
	{
		cout << r->C->N << " ";
		for (Node2*j = r->R; j != r; j = j->R) // all the nodes in row
		{
			cout << j->C->N << " ";
		}
		cout << "\n";
	}

	cout << "]\n";
}

// consider:
// - rework data structure so static_casts are not needed
