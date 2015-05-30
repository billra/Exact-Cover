// modified dancing links solver implementation: dancing indices, use vector index instead of node pointer
// Bill Ola Rasmussen

#include "dix.h"
#include <stdexcept>
#include <iostream>
#include <limits>
using namespace std;

// ---------- build data structure from input ----------

const DIX::TileNode DIX::SEP{ numeric_limits<DIX::TI>::max(),numeric_limits<DIX::TI>::max(),0 }; // tile separator

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
	_tile.push_back(SEP); // unused tile so that head and tile columns have same array index
	for (TI i(0); i < pc+sc; ++i) {
		_tile.push_back({ i, i, 1+i }); // link to self, translate 0 based column input to 1 based internal representation
	}
	_tile.push_back(SEP);
}

void DIX::Row(const unsigned int col)
{
	_tile.push_back(SEP);
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

	// insert a new node: overwrite the separator, then push a new one
	if (!(SEP==_tile.back())) { throw(runtime_error("tile separator not found")); }
	_tile.back()={ iU,_tile[iU].D,c };
	_tile[_tile[iU].D].U = _tile.size()-1; // point next to this
	_tile[iU].D = _tile.size()-1; // point previous to this
	_tile.push_back(SEP);
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
	if (!(SEP==_tile.back())) { throw(runtime_error("tile separator not found")); }

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

void DIX::CoverNode(const TI& c)
{
	--_head[_tile[c].C].N; // inform column head that it has one less node
	_tile[  _tile[c].D].U = _tile[c].U; // remove node from column
	_tile[  _tile[c].U].D = _tile[c].D;
}

void DIX::UncoverNode(const TI& c)
{
	++_head[_tile[c].C].N; // inform column head that node came back
	_tile[  _tile[c].D].U = c; // insert node back into column
	_tile[  _tile[c].U].D = c;
}

void DIX::Cover(const TI& c) // remove all tiles covering node, remove node
{
	// remove self from head node list
	_head[_head[c].R].L = _head[c].L;
	_head[_head[c].L].R = _head[c].R;

	// cover tiles
	for (TI i(_tile[c].D); i; i = _tile[i].D) { // all tiles having nodes in this column
		for (TI j(i - 1); _tile[j].C; --j) { CoverNode(j); } // all nodes to left of column
		for (TI j(i + 1); _tile[j].C; ++j) { CoverNode(j); } // all nodes to right of column
	}
}

void DIX::Uncover(const TI& c)
{
	// reinsert self into head node list
	_head[_head[c].R].L = c;
	_head[_head[c].L].R = c;

	// uncover tiles
	for (TI i(_tile[c].U); i; i = _tile[i].U) { // all tiles having nodes in this column, reverse order
		for (TI j(i - 1); _tile[j].C; --j) { UncoverNode(j); } // all nodes to left of column
		for (TI j(i + 1); _tile[j].C; ++j) { UncoverNode(j); } // all nodes to right of column
	}
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
