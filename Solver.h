// interface for solve operations
// Bill Ola Rasmussen

struct Solver {
    enum class Event{Begin,Soln,End};
    virtual void Init(int pc, int sc, void(*CallBack)(Event))=0; // primary and secondary constraint count
    virtual void Row(int col)=0; // start a new row with a column constraint
    virtual void Col(int col)=0; // insert column constraint in current row
    virtual void Solve()=0;
};

