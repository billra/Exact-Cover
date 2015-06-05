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
}

void ACX::Row(const unsigned int /*col*/)
{
}

void ACX::Col(const unsigned int /*col*/)
{
}

// ---------- solve exact cover problem ----------

void ACX::Solve(const bool showSoln, std::function<void(Event)> CallBack)
{
	_show = showSoln;
	_notify = CallBack;

	cout << "ACX::Solve with "  " tiles\n";
}
