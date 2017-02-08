// algorithm xt solver interface
// Bill Ola Rasmussen

#include "Solver.h"

class AXT:public Solver{
public:
	AXT()=default;
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	AXT&operator=(const AXT&)=delete; // no assignment
	AXT(const AXT&)=delete; // no copy constructor
};
