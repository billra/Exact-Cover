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
	if (!_start_board.empty()) { throw(runtime_error("board already initialized")); }
	_start_board.resize(pc + sc, 0); // all cover counts start at zero
}

void ACX::Row(const unsigned int col)
{
	_start_tiles.push_back(Tile()); // start a new tile
	Col(col);
}

void ACX::Col(const unsigned int col)
{
	if (col >= _start_board.size()) { throw(runtime_error("column index for tile out of board range")); }
	++_start_board[col]; // increase cover count
	_start_tiles.back().push_back(col);
}

void ACX::ShrinkToFit()
{
	// call shrink_to_fit() on all vectors
	_start_board.shrink_to_fit();
	for (auto& t : _start_tiles) { t.shrink_to_fit(); }
	_start_tiles.shrink_to_fit();
}

// ---------- solve exact cover problem ----------

void ACX::Solve(const bool showSoln, std::function<void(Event)> CallBack)
{
	_show = showSoln;
	_notify = CallBack;
	cout << "ACX::Solve, board size: " << _start_board.size() << ", tiles: " << _start_tiles.size() << "\n";
	ShrinkToFit(); // vector sizes are now unchanging, so trim extra space
	Tiles soln;
	Search(soln, _start_board, _start_tiles);
}

void ACX::Search(Tiles& soln, const Board& board, const Tiles& tiles)
{
	const auto col(ChooseColumn(board));
	if (!_start_board[col]) { return; } // a column could not be covered with remaining tiles, abort this search branch
	// todo: check done for _pc constraints

	const auto choices(Covers(col,tiles)); // tiles which cover col
	for (const auto& iChoose : choices) {
		const auto& choice(tiles[iChoose]);
		soln.push_back(choice);
		Board newBoard(board);
		Tiles newTiles;
		LayTile(newBoard, newTiles, board, tiles, choice);

		// todo: recurse, check...

		soln.pop_back();
	}
}

void ACX::LayTile(Board& newBoard, Tiles& newTiles, const Board& board, const Tiles& tiles, const Tile& choice) const
{
	// only copy tiles which do not collide
	for (TI i(0); i < tiles.size(); ++i) { // all tiles
		const auto& check(tiles[i]);
		if (Intersect(choice, check)) { 
			Subtract(newBoard, check); // remove board coverage of discarded tile
			continue; 
		}
		newTiles.push_back(check);
	}

	// choice tile board positions are now at zero coverage
	// mark covered board squares
	for (TI i(0); i < choice.size(); ++i) {
		newBoard[i]= numeric_limits<TI>::max(); 
	}
}

bool ACX::Intersect(const Tile& tile1, const Tile& tile2) const
{
	for (TI j(0), k(0); j < tile1.size() && k < tile2.size();) { // each square in tiles
		const TI& node1(tile1[j]);
		const TI& node2(tile2[k]);
		if      (node1 < node2) { ++j; }
		else if (node1 > node2) { ++k; }
		else { return true; } // collision
	}
	return false;
}

void ACX::Subtract(Board& board, const Tile& tile) const
{
	for (TI i(0); i < tile.size(); ++i) { // all nodes in tile
		--board[tile[i]];
	}
}

ACX::TI ACX::ChooseColumn(const Board& board) const
{
	// minimize search space by selecting most constrained column
	TI iMin(0); // assume first square on board
	for (TI i(1); i < board.size(); ++i) {
		if (board[iMin] > board[i]) { iMin = i; }
	}
	return iMin;
}

vector<ACX::TI> ACX::Covers(const TI col, const Tiles& tiles) const
{
	// linear search for tiles covering col (as fast or faster than following pointers? we will see...)
	Tile vt;
	for (TI i(0); i < tiles.size(); ++i) { // all tiles
		const Tile& tile(tiles[i]);
		for (TI j(0); j < tile.size(); ++j) { // each square in tile
			if (col == tile[j]) {
				vt.push_back(i);
				break; // col found in this tile, move immediately to next tile
			}
		}
	}
	return vt; // elided
}

