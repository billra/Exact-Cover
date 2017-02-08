// algorithm xt solver implementation
// Bill Ola Rasmussen

#include "axt.h"
#include <iostream>
#include <assert.h>
using namespace std;

Tiles::Tiles(const VVUI& vtile) : tileStorage(vtile.size())
{
	assert(tileStorage.size() > 1); // this implementation needs minimum two tiles
	const auto iLast(tileStorage.size()-1);
	// link up tiles
	for (VVUI::size_type i(0); i < tileStorage.size(); ++i) {
		tileStorage[i].prevTile = 0 == i ? &tileStorage[iLast] : &tileStorage[i - 1];
		tileStorage[i].nextTile = iLast == i ? &tileStorage[0] : &tileStorage[i + 1];
	}
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void AXT::Init(const unsigned int pc, const unsigned int sc)
{
	npc = pc;
	constraint.resize(pc+sc); // value-initialized to zero
}

void AXT::Row(const unsigned int col)
{
	vtile.push_back(vector<unsigned int>());
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

