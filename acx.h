// array copy solver
// motivation: sequential access is very fast on modern architectures
// Bill Ola Rasmussen

#include "Solver.h"

class ACX : public Solver {
	using TI = unsigned int; // index type
	bool _show;
	std::function<void(Event)> _notify;
public:
	ACX(){}
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	ACX&operator=(const ACX&)=delete; // no assignment
	ACX(const ACX&)=delete; // no copy constructor
};
