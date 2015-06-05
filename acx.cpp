// array copy solver
// Bill Ola Rasmussen

#include "acx.h"
#include <stdexcept>
#include <iostream>
using namespace std;

// ---------- build data structure from input ----------

void ACX::Init(const unsigned int pc, const unsigned int sc)
{
	// primary constraint: cover this exactly once
	// secondary constraint: cover this at most once
	if (0 == pc+sc) { throw(runtime_error("zero constraint count")); }
	_pc = pc;
	if (!_board.empty()) { throw(runtime_error("board already initialized")); }
	_board.resize(pc + sc, 0); // all cover counts start at zero
}

void ACX::Row(const unsigned int col)
{
	_tile.push_back(Tile()); // start a new tile
	Col(col);
}

void ACX::Col(const unsigned int col)
{
	if (col >= _board.size()) { throw(runtime_error("column index for tile out of board range")); }
	++_board[col]; // increase cover count
	_tile.back().push_back(col);
}

void ACX::ShrinkToFit()
{
	// call shrink_to_fit() on all vectors
	_board.shrink_to_fit();
	for (auto& t : _tile) { t.shrink_to_fit(); }
	_tile.shrink_to_fit();
}

// ---------- solve exact cover problem ----------

void ACX::Solve(const bool showSoln, std::function<void(Event)> CallBack)
{
	_show = showSoln;
	_notify = CallBack;
	cout << "ACX::Solve, board size: " << _board.size() << ", tiles: " << _tile.size() << "\n";
	ShrinkToFit(); // vector sizes are now unchanging, so trim extra space
}
