// array copy solver
// Bill Ola Rasmussen

#include "acx.h"
#include <stdexcept>
#include <iostream>
#include <limits>
using namespace std;

// ---------- build data structure from input ----------

void ACX::Init(const unsigned int pc, const unsigned int sc)
{
	// primary constraint: cover this exactly once
	// secondary constraint: cover this at most once
	if (0 == pc + sc) { throw(runtime_error("zero constraint count")); }
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
	Search(_board, _tile);
}

void ACX::Search(const vector<TI>& board, const vector<Tile>& tile)
{
	const auto col(ChooseColumn(board));
	if (!_board[col]) { return; } // a column could not be covered with remaining tiles, abort this search branch

	const auto choices(Covers(col)); // tiles which cover col
	for (const auto& iTile : choices) {
		const auto newBoard(TileBoard(board, tile[iTile]));
		const auto newTile(RemoveConflicts(tile, iTile));
		// recurse...
	}
}

std::vector<ACX::TI> ACX::TileBoard(const vector<TI>& board, const vector<TI>& tile) const
{
	// todo
	return vector<TI>();
}

std::vector<ACX::Tile> ACX::RemoveConflicts(const vector<Tile>& tile, const TI iTile) const
{
	// todo
	return vector<ACX::Tile>();
}

ACX::TI ACX::ChooseColumn(const vector<TI>& board) const
{
	// minimize search space by selecting most constrained column
	TI iMin(0); // assume first square on board
	for (TI i(1); i < board.size(); ++i) {
		if (board[iMin] > board[i]) { iMin = i; }
	}
	return iMin;
}

vector<ACX::TI> ACX::Covers(const TI col) const
{
	// linear search for tiles covering col (as fast or faster than following pointers? we will see...)
	vector<TI> vt;
	for (TI i(0); i < _tile.size(); ++i) {
		for (TI j(0); j < _tile[i].size(); ++j) {
			if (col == _tile[i][j]) {
				vt.push_back(i);
				break; // found in this tile, move immediately to next tile
			}
		}
	}
	return vt; // elided
}

