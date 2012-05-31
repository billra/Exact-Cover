// interface for solve operations
// Bill Ola Rasmussen

struct Solver {
    virtual void Init(int pc, int sc)=0; // primary and secondary constraint count
    virtual void Row(int col)=0; // start a new row with a column constraint
    virtual void Col(int col)=0; // insert column constraint in current row
    virtual void Solve()=0;
};

