// modified dancing links solver implementation: dancing indices, use vector index instead of node pointer
// Bill Ola Rasmussen

#include "dix.h"
#include <stdexcept>
#include <iostream>
#include <limits>
using namespace std;

// ---------- build data structure from input ----------

void DIX::Init(const unsigned int pc, const unsigned int sc)
{
	// primary constraint: cover this exactly once
	// secondary constraint: cover this at most once
	if (0 == pc+sc) { throw(runtime_error("zero constraint count")); }

	// create head row vector
	_head.resize(1+pc+sc); // head node of head nodes, primary constraints, secondary constraints
	_head[0] = { pc, 1, numeric_limits<TI>::max() }; // head node of head nodes circular link: beginning to end
	for (TI i(1); i < 1+pc; ++i) {
		_head[i] = { i-1, i+1, 0 }; // primary constraints: link to both neighbors
	}
	if (pc) {
		_head[pc].R = 0; // last primary constraint node, link end to beginning
	}
	for (TI i(1+pc); i < 1+pc+sc; ++i) {
		_head[i] = { i, i, 0 }; // secondary constraints: link to self
	}

	// create complete tile array to serve as up/down pointers for head nodes
	// one smaller than _head because no head node of head nodes
	for (TI i(0); i < pc+sc; ++i) {
		_tile.push_back({ i, i, 1+i }); // link to self, translate 0 based column input to 1 based internal representation
	}
}

void DIX::Row(const unsigned int col)
{
	Col(col);
}

void DIX::Col(const unsigned int external_col_id) // external col starts at 0
{
	// adjust index
	const unsigned int c(1+external_col_id); // translate 0 based column input to 1 based internal representation

	// increment node count head array
	if (c>=_head.size()) { throw(runtime_error("column index out of range")); }
	_head[c].N++; // increase cover count

	// find previous node in same column
	TI iU(_tile.size()-1);
	for(;iU;--iU){ // reverse node/tile search
		if(c==_tile[iU].C){ break; }
	}
	if (c!=_tile[iU].C) { throw(runtime_error("previous node for column not found")); }

	// insert a new node
	_tile.push_back({ iU,_tile[iU].D,c });
	_tile[_tile[iU].D].U = _tile.size()-1; // point next to this
	_tile[iU].D = _tile.size()-1; // point previous to this
}

// ---------- solve exact cover problem ----------

void DIX::ShrinkToFit()
{
	// call shrink_to_fit() on all vectors
	_head.shrink_to_fit();
	_tile.shrink_to_fit();
}

void DIX::Solve(const bool showSoln, std::function<void(Event)> CallBack)
{
	_show = showSoln;
	_notify = CallBack;
	cout << "DIX::Solve with " << _head.size() << " head nodes, " << _tile.size() << " tiles\n";

	ShrinkToFit(); // vector sizes are now unchanging, so trim extra space
	const auto check_head(_head); // save for later integrity check
	const auto check_tile(_tile);

	vector<TI> soln;
	_notify(Event::Begin);
	Search(soln);
	_notify(Event::End);

	if (check_head != _head) { throw(runtime_error("head node structure integrity failure")); }
	if (check_tile != _tile) { throw(runtime_error("tile node structure integrity failure")); }
	cout << "Node structure integrity verified.\n";
}

void DIX::Search(vector<TI>& soln)
{
	if (!_head[0].R) // head node of head nodes points to itself, indicates all constraints are met
	{
		_notify(Event::Soln);
		if (_show) { ShowSoln(soln); }
		return;
	}

	const TI c(ChooseColumn()); // get _head vector index of minimally covered column
	if (!c) { return; } // a column could not be covered with remaining tiles, abort this search branch
	// invariant: c > 0

	Cover(c); // take all tiles covering this node out of play

	//for (Node*r = c->D; r != c; r = r->D) // all the rows in column c
	//{
	//	O.emplace_back(r);
	//	for (Node*j = r->R; j != r; j = j->R) // all the nodes in row
	//	{
	//		Cover(j->C);
	//	}

	Search(soln);

	//	O.pop_back(); // implements: set r ? O sub k and c ? C[r];
	//	for (Node*j = r->L; j != r; j = j->L) // all the nodes in row
	//	{
	//		Uncover(j->C);
	//	}
	//}
	Uncover(c);
}

void DIX::Cover(const TI & c) // remove all tiles covering node, remove node
{
	// remove self from head node list
	_head[_head[c].R].L = _head[c].L;
	_head[_head[c].L].R = _head[c].R;

	// remove tiles
//	for (TI i(_tile[0][c].D); i;) {
	//for (Node*i = c->D; i != c; i = i->D) // all tiles having nodes in this column
	//{
//		for (TI j(0); j < _tile[i].size(); ++j) { // remove all nodes in tile outside column c
	//	for (Node*j = i->R; j != i; j = j->R) 
	//	{
//			auto& node(_tile[i][j]);
//			if (j == c) {
//				i = node.D; // use column c for link to next tile
//			}
//			else { // remove node from column
//				_tile[node.D][???].U = node.U;  // problem: need to search destination array!
//			}
	//		// remove node from column
	//		j->D->U = j->U;
	//		j->U->D = j->D;
	//		// inform column head that it has one less node
	//		--(j->C->S);
//		}
//	}
}

void DIX::Uncover(const TI & c)
{
	// process column
	//for (Node*i = c->U; i != c; i = i->U) // all rows having nodes in this column, reverse order
	//{
	//	for (Node*j = i->L; j != i; j = j->L) // all _other_ nodes in this row, reverse order
	//	{
	//		// inform column head that its node came back
	//		++(j->C->S);
	//		// insert node back into column
	//		j->D->U = j;
	//		j->U->D = j;
	//	}
	//}

	// reinsert self into head node list
	_head[_head[c].R].L = c;
	_head[_head[c].L].R = c;
}

DIX::TI DIX::ChooseColumn() const
{
	// minimize search space by selecting most constrained column
	TI iMin(0); // head node: N == numeric_limits<TI>::max()
	for (TI ih(_head[0].R); ih; ih = _head[ih].R) {
		if (!_head[ih].N) { return 0; } // early return, no way to cover a column
		if (_head[ih].N < _head[iMin].N) { iMin = ih; }
	}
	return iMin;
}

void DIX::ShowSoln(const vector<TI>& soln)const
{
	cout << "[\n";
	for (const auto& iRow : soln) // for each tile in the solution
	{
		cout << iRow << endl; // todo: rework
		//for (const auto& iCol : _tile[iRow]) // print out all the nodes in the tile
		//{
		//	cout << iCol.C-1 << " "; // internal column is 1 based, external representation is 0 based
		//}
		//cout << "\n";
	}
	cout << "]\n";
}
