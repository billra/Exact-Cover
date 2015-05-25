// modified dancing links solver implementation: dancing indices, use vector index instead of node pointer
// Bill Ola Rasmussen
#include "Solver.h"
#include <vector>

using TI = unsigned int; // index type

class DIX : public Solver {
	struct HeadNode { TI L, R, N; }; // column to left, column to right, count of nodes covering column
	struct TileNode { TI U, D, C; }; // row up, row down, column covered by the node
	std::vector<HeadNode> _head;
	std::vector<std::vector<TileNode>> _tile;
	bool _show;
	std::function<void(Event)> _notify;
public:
	DIX(){}
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	DIX&operator=(const DIX&)=delete; // no assignment
	DIX(const DIX&)=delete; // no copy constructor
};
