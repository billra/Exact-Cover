// modified dancing links solver implementation: dancing indices, use vector index instead of node pointer
// Bill Ola Rasmussen
#include "Solver.h"
#include <vector>

class DIX : public Solver {
	using TI = unsigned int; // index type
	struct HeadNode { 
		TI L, R, N;  // column to left, column to right, count of nodes covering column
		bool operator==(const HeadNode&rhs)const{return rhs.L==L && rhs.R==R && rhs.N==N;} // used in integrity check
	};
	struct TileNode { 
		TI U, D, C; // row up, row down, column covered by the node: index starting at 1 to match HeadNode vector
		bool operator==(const TileNode&rhs)const{return rhs.U==U && rhs.D==D && rhs.C==C;} // used in integrity check
	};
	const static TileNode SEP; // tile separator
	std::vector<HeadNode> _head;
	std::vector<TileNode> _tile; // key concept: at solve time, the relationship between tile nodes is static
	bool _show;
	std::function<void(Event)> _notify;
	void ShrinkToFit(); // optional optimization
	void Search(std::vector<TI>& soln);
	void CoverNode(const TI& c);
	void UncoverNode(const TI& c);
	void Cover(const TI& c);
	void Uncover(const TI& c);
	TI ChooseColumn()const;
	void ShowSoln(const std::vector<TI>& soln)const;
public:
	DIX(){}
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	DIX&operator=(const DIX&)=delete; // no assignment
	DIX(const DIX&)=delete; // no copy constructor
};
