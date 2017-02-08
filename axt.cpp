// algorithm xt solver implementation
// Bill Ola Rasmussen

#include "axt.h"
#include <iostream>
#include <assert.h>
using namespace std;

bool intersect(const VUI& vc1, const VUI& vc2)
{
	for (const auto& c1 : vc1) {
		for (const auto& c2 : vc2) {
			if (c1 == c2) { return true; }
		}
	}
	return false;
}

unsigned int countTileConflicts(const VVUI& vtile, const VUI& target)
{
	unsigned int conflicts(0);
	for (const auto& tile : vtile) {
		if (intersect(target, tile)) { ++conflicts; }
	}
	return --conflicts;
}

Tiles::Tiles(const VVUI& vtile) : tileStorage(vtile.size())
{
	assert(vtile.size() == tileStorage.size());
	const auto size(vtile.size());
	assert(size > 1); // this implementation needs minimum two tiles
	const auto iLast(tileStorage.size() - 1);
	// link up tiles
	for (VVUI::size_type i(0); i < size; ++i) {
		tileStorage[i].prevTile = 0 == i ? &tileStorage[iLast] : &tileStorage[i - 1];
		tileStorage[i].nextTile = iLast == i ? &tileStorage[0] : &tileStorage[i + 1];
	}
	// conflict storage
	unsigned int conflictCount(0);
	for (VVUI::size_type i(0); i < size; ++i) {
		const auto nConflict(countTileConflicts(vtile, vtile[i]));
		cout << "nConflict " << nConflict << endl;
		conflictCount += nConflict;
		tileStorage[i].conflictStorage.resize(nConflict);
	}
	cout << "conflict total: " << conflictCount << endl;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void AXT::Init(const unsigned int pc, const unsigned int sc)
{
	npc = pc;
	constraint.resize(pc+sc); // value-initialized to zero
}

void AXT::Row(const unsigned int col)
{
	vtile.push_back(VUI());
	itCurTile = prev(vtile.end());
	Col(col);
}

void AXT::Col(const unsigned int col)
{
	assert(col < constraint.size());
	itCurTile->push_back(col);
	++constraint[col];
}

void AXT::Solve(const bool showSoln, function<void(Event)> CallBack)
{
	show = showSoln;
	Notify = CallBack;

	cout << "AXT::Solve with " << vtile.size() << " tiles\n";
	Tiles tiles(vtile); // solving structure

	Notify(Event::Begin);
	// Search
	Notify(Event::End);
}

