// interface for solve operations
// Bill Ola Rasmussen

class Solver {
protected:
	int _pc,_sc;
public:
	Solver(int pc, int sc):_pc(pc),_sc(sc) // primary and secondary constraint count
	{
	}
	virtual ~Solver()
	{
	}
	virtual void Row()=0; // start a new row
	virtual void Col(int column)=0; // insert column constraint in current row
	virtual void Solve()=0;
};

