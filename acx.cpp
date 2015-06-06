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
	if (!pc) { throw(runtime_error("no primary constraints")); }
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
	_notify(Event::Begin);
	Search(soln, _start_board, _start_tiles);
	_notify(Event::End);

}

void ACX::Search(Tiles& soln, const Board& board, const Tiles& tiles)
{
	const auto col(ChooseColumn(board));
	if (!board[col]) { return; } // a column could not be covered with remaining tiles, abort this search branch
	if (numeric_limits<TI>::max() == board[col]) { // all primary constraint board positions are covered
		_notify(Event::Soln);
		if (_show) { ShowSoln(soln); }
		return;
	}

	const auto choices(Covers(col,tiles)); // tiles which cover col
	for (const auto& iChoose : choices) {
		const auto& choice(tiles[iChoose]);
		Board newBoard(board); // start with board and subtract
		Tiles newTiles; // start empty and add tiles
		LayTile(newBoard, newTiles, tiles, choice);

		soln.push_back(choice); // todo: could push back a pointer instead of a tile
		Search(soln, newBoard, newTiles);
		soln.pop_back();
	}
}

void ACX::LayTile(Board& newBoard, Tiles& newTiles, const Tiles& tiles, const Tile& choice) const
{
	// only copy tiles which do not collide
	for (const auto& tile : tiles) { // all tiles
		if (Intersect(choice, tile)) {
			Subtract(newBoard, tile); // remove board coverage of discarded tile
			continue;
		}
		newTiles.push_back(tile);
	}

	// choice tile board positions are now at zero coverage
	// mark covered board squares
	for (const auto& pos : choice) {
		newBoard[pos] = numeric_limits<TI>::max();
	}
}

bool ACX::Intersect(const Tile& tile1, const Tile& tile2) const
{
	for (TI j(0), k(0); j < tile1.size() && k < tile2.size();) { // each square in tiles
		const TI& pos1(tile1[j]);
		const TI& pos2(tile2[k]);
		if      (pos1 < pos2) { ++j; }
		else if (pos1 > pos2) { ++k; }
		else { return true; } // collision
	}
	return false;
}

void ACX::Subtract(Board& board, const Tile& tile) const
{
	for (const auto& pos : tile) { // all positions in tile
		--board[pos];
	}
}

ACX::TI ACX::ChooseColumn(const Board& board) const
{
	// minimize search space by selecting most constrained column
	TI iMin(0); // assume first square on board
	for (TI i(1); i < _pc; ++i) { // limit search to primary constraints
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
		for (const auto& pos : tile) { // each position in tile
			if (col == pos) {
				vt.push_back(i);
				break; // col found in this tile, move immediately to next tile
			}
		}
	}
	return vt; // elided
}

void ACX::ShowSoln(const Tiles& soln)
{
	cout << "[\n";
	for (const auto& tile : soln) { // all tiles
		for (const auto& pos : tile) { // each position in tile
			cout << pos << " ";
		}
		cout << "\n";
	}
	cout << "]\n";
}

