// interface for solve operations
// Bill Ola Rasmussen
#pragma once

#include <functional>

struct Solver {
	enum class Event : char {Begin,Soln,End};
	virtual void Init(const unsigned int pc, const unsigned int sc) = 0; // primary and secondary constraint count
	virtual void Row(const unsigned int col)=0; // start a new row with a column constraint
	virtual void Col(const unsigned int col)=0; // insert column constraint in current row
	virtual void Solve(const bool showSoln, std::function<void(Event)>CallBack)=0; // show solutions, callback for solution events
};

