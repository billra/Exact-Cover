// dlx solver interface
// Bill Ola Rasmussen
#include "Solver.h"
class DLX : public Solver {
public:
	DLX(int pc, int sc);
	~DLX();
	void Row();
	void Col(int column);
	void Solve();
};

