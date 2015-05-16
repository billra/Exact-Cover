// interface for solve operations
// Bill Ola Rasmussen

#include <functional>

struct Solver {
	enum class Event : char {Begin,Soln,End};
	virtual void Init(const int pc, const int sc, std::function<void(Event)>) = 0; // primary and secondary constraint count
	virtual void Row(const int col)=0; // start a new row with a column constraint
	virtual void Col(const int col)=0; // insert column constraint in current row
	virtual void Solve(const bool showSoln=true)=0;
};

