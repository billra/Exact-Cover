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
	// Minimize work by finding least covered column. A set of tile choices covers
	// this column. Minimize work by removing these choices once instead of each
	// time through the choice loop.
	Board boardNC(board); // start with board and subtract
	TilesIdxs tilesidxsNC(tilesidxs); // start full and remove
	const auto choices(RemoveTiles(boardNC, tilesidxsNC, col)); // remove tiles covering col, return list of tiles

	for (const auto& tilesidx : choices) {
		const auto& choice(_start_tiles[tilesidx]);
		const auto removed(RemoveTiles(boardNC, tilesidxsNC, choice));
		MarkBoard(boardNC, choice, numeric_limits<TI>::max());
		// slower:
		//for (const auto& pos : boardNC) { // early check:
		//	if (!pos) { continue; } // impossible to cover board after making choice
		//}

		soln.push_back(tilesidx);
		Search(soln, boardNC, tilesidxsNC);
		soln.pop_back();

		MarkBoard(boardNC, choice, 0);
		AddTiles(boardNC, tilesidxsNC, removed);
	}
}

ACX::TilesIdxs ACX::RemoveTiles(Board& board, TilesIdxs& tilesidxs, const TI col) const
{
	TilesIdxs removed;
	for (TI i(0); i < tilesidxs.size();) { // tiles which intersect col and are removed
		const auto& tilesidx(tilesidxs[i]);
		const auto& tile(_start_tiles[tilesidx]);
		if (!Intersect(tile, col)) { ++i;  continue; }
		removed.push_back(tilesidx);
		Subtract(board, tile);
		tilesidxs.erase(tilesidxs.begin() + i); // expensive? -> no
	}
	return removed; // elided
}

ACX::TilesIdxs ACX::RemoveTiles(Board& board, TilesIdxs& tilesidxs, const Tile& choice) const
{
	TilesIdxs removed;
	for (TI i(0); i < tilesidxs.size();) { // tiles which intersect tile and are removed
		const auto& tilesidx(tilesidxs[i]);
		const auto& tile(_start_tiles[tilesidx]);
		if (!Intersect(tile, choice)) {	++i; continue;	}
		removed.push_back(tilesidx);
		Subtract(board, tile);
		tilesidxs.erase(tilesidxs.begin() + i); // expensive? -> no
	}
	return removed; // elided
}

void ACX::AddTiles(Board& board, TilesIdxs& tilesidxs, const TilesIdxs& removed) const
{
	for (TI i(0); i < removed.size();) { // tiles which intersect col and were removed
		const auto& tilesidx(tilesidxs[i]);
		const auto& tile(_start_tiles[tilesidx]);
		Add(board, tile);
		tilesidxs.push_back(i);
	}
}

bool ACX::Intersect(const Tile& tile, const TI col) const
{
	for (const auto& pos : tile) {
		if (col == pos) { return true; }
	}
	return false;
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

void ACX::Add(Board & board, const Tile & tile) const
{
	for (const auto& pos : tile) { // all positions in tile
		++board[pos];
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

