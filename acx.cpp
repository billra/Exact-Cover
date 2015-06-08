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
	TilesIdxs soln;
	TilesIdxs tilesidxs; // index into _start_tiles
	for (TI i(0); i < _start_tiles.size(); ++i) {
		tilesidxs.push_back(i); // include all tiles in possibility list
	}
	_notify(Event::Begin);
	Search(soln, _start_board, tilesidxs);
	_notify(Event::End);
}

void ACX::Search(TilesIdxs& soln, const Board& board, const TilesIdxs& tilesidxs) const
{
	const auto col(ChooseColumn(board));
	if (!board[col]) { return; } // a column could not be covered with remaining tiles, abort this search branch
	if (numeric_limits<TI>::max() == board[col]) { // all primary constraint board positions are covered
		_notify(Event::Soln);
		if (_show) { ShowSoln(soln); }
		return;
	}

	const auto choices(Covers(col,tilesidxs)); // tiles which cover col
	// todo: like original algorithm, can take choices out of play here
	for (const auto& tilesidx : choices) {
		const auto& choice(_start_tiles[tilesidx]);
		Board newBoard(board); // start with board and subtract
		TilesIdxs newTilesidxs(tilesidxs); // start full and remove
		LayTile(newBoard, newTilesidxs, choice);
		MarkBoard(newBoard, choice, numeric_limits<TI>::max());

		soln.push_back(tilesidx);
		Search(soln, newBoard, newTilesidxs);
		soln.pop_back();
	}
	// todo: like original algorithm, can put choices back into play here
}

void ACX::LayTile(Board& newBoard, TilesIdxs& newTilesidxs, const Tile& choice) const
{
	// only copy tiles which do not collide
	for (TI i(0); i < newTilesidxs.size();) {
		const auto&tilesidx(newTilesidxs[i]);
		const auto& tile(_start_tiles[tilesidx]); // extra work compared to passing index instead of tile
		if (!Intersect(choice, tile)) {	++i; continue;	}
		Subtract(newBoard, tile); // remove board coverage of discarded tile
		newTilesidxs.erase(newTilesidxs.begin() + i); // expensive? -> no
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

void ACX::MarkBoard(Board & board, const Tile & choice, const TI val) const
{
	// choice tile board positions are now at zero coverage
	// mark covered board squares
	for (const auto& pos : choice) {
		board[pos] = val;
	}
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

ACX::TilesIdxs ACX::Covers(const TI col, const TilesIdxs& tilesidxs) const
{
	// linear search for tiles covering col (as fast or faster than following pointers? we will see...)
	TilesIdxs vti;
	for (const auto& tilesidx : tilesidxs) { // all tile indices
		const Tile& tile(_start_tiles[tilesidx]);
		for (const auto& pos : tile) { // each position in tile
			if (col == pos) {
				vti.push_back(tilesidx);
				break; // col found in this tile, move immediately to next tile
			}
		}
	}
	return vti; // elided
}

void ACX::ShowSoln(const TilesIdxs& soln) const
{
	cout << "[\n";
	for (const auto& tilesidx : soln) { // all solution tile indices
		const Tile& tile(_start_tiles[tilesidx]);
		for (const auto& pos : tile) { // each position in tile
			cout << pos << " ";
		}
		cout << "\n";
	}
	cout << "]\n";
}

