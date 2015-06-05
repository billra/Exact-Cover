// array copy solver
// motivation: sequential access is very fast on modern architectures
// Bill Ola Rasmussen

#include "Solver.h"
#include <vector>
//#include <cstdint>

class ACX : public Solver {
	// C++11 <cstdint> types:
	//	uint8_t		0 to           255	1 byte
	//	uint16_t	0 to        65,535	2 bytes
	//	uint32_t	0 to 4,294,967,295	4 bytes, same as Microsoft unsigned int
	using TI = unsigned int; // index type
	unsigned int _pc; // primary constraints, size of _board or less
	std::vector<TI> _board;
	using Tile = std::vector<TI>; // single tile
	std::vector<Tile> _tile; // all available tiles
	bool _show;
	std::function<void(Event)> _notify;
	void ShrinkToFit(); // optional optimization
	void Search(const std::vector<TI>& board, const std::vector<Tile>& tile);
	std::vector<TI> TileBoard(const std::vector<TI>& board, const std::vector<TI>& tile) const;
	std::vector<Tile> RemoveConflicts(const std::vector<Tile>& tile, const TI iTile) const;
	TI ChooseColumn(const std::vector<TI>& board) const;
	std::vector<TI> Covers(const TI col) const;
public:
	ACX() {}
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	ACX&operator=(const ACX&) = delete; // no assignment
	ACX(const ACX&) = delete; // no copy constructor
};
