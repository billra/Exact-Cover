// algorithm xt solver implementation
// Bill Ola Rasmussen

#include "axt.h"
#include <iostream>
#include <assert.h>
using namespace std;

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

	Notify(Event::Begin);
	// Search
	Notify(Event::End);
}
