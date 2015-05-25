// modified dancing links solver implementation: dancing indices, use vector index instead of node pointer
// Bill Ola Rasmussen

#include "dix.h"
#include <stdexcept>
#include <iostream>
using namespace std;

// ---------- build data structure from input ----------

void DIX::Init(const unsigned int pc, const unsigned int sc)
{
	if (0 == pc+sc) { throw(runtime_error("zero constraint count")); }

	// create head row vector
	_head.resize(1+pc+sc); // head node of head nodes, primary constraints, secondary constraints
	_head[0] = { pc, 1, 0 }; // head node of head nodes circular link: beginning to end
	for (TI i(1); i < 1+pc; ++i) {
		_head[i] = { i-1, i+1, 0 }; // primary constraints: link to both neighbors
	}
	if (pc) {
		_head[pc].R = 0; // last primary constraint node, link end to beginning
	}
	for (TI i(1+pc); i < 1+pc+sc; ++i) {
		_head[i] = { i, i, 0 }; // secondary constraints: link to self
	}

	// create complete first row on tile array to serve as up/down pointers for head nodes
	_tile.emplace_back(vector<TileNode>(pc+sc)); // _tile[0] is one smaller than _head because no head node of head nodes
	for (TI i(0); i < pc+sc; ++i) {
		_tile.back()[i] = { 0, 0, i }; // all constraints: link to self
	}
}

void DIX::Row(const unsigned int col)
{
	if (1+col>=_head.size()) { throw(runtime_error("column index out of range")); }
	_tile.emplace_back(vector<TileNode>()); // empty vector
	Col(col);
}

void DIX::Col(const unsigned int col)
{
	if (1+col>=_head.size()) { throw(runtime_error("column index out of range")); }
	_head[1+col].N += 1; // increase cover count
	auto& iPrevTile(_tile[0][col].U); // previous tile with node in column col
	auto& prevTile(_tile[iPrevTile]); // tile with node before new tile
	TI iCol(0); // index of node in column col in prevTile
	for (; iCol < prevTile.size() && col!=prevTile[iCol].C; ++iCol) {}
	if (prevTile.size()==iCol) { throw(runtime_error("column index not found in previous tile")); }
	const auto iRow(_tile.size()-1); // index of current insert row
	prevTile[iCol].D = iRow; // point previous last to new last
	_tile.back().push_back({ iPrevTile,0,col }); // link node to head row in _tile
	iPrevTile = iRow; // update head to index new last
}

// ---------- solve exact cover problem ----------

void DIX::Solve(const bool showSoln, std::function<void(Event)> CallBack)
{
	_show = showSoln;
	_notify = CallBack;
	cout << "DIX::Solve with " << _head.size() << " head nodes, " << _tile.size() << " tiles\n";

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
	// ... magic happens here ...

	ShowSoln(soln); // dummy
}

void DIX::ShowSoln(const vector<TI>& soln)const
{
	cout << "[\n";
	for (const auto& iRow : soln) // for each tile in the solution
	{
		for (const auto& iCol : _tile[iRow]) // print out all the nodes in the tile
		{
			cout << iCol.C << " ";
		}
		cout << "\n";
	}
	cout << "]\n";
}
