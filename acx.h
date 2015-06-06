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
	unsigned int _pc; // primary constraints, size of _start_board or less
	using Board = std::vector<TI>;
	Board _start_board;
	using Tile = std::vector<TI>;
	using Tiles = std::vector<Tile>;
	Tiles _start_tiles; // all available tiles
	bool _show;
	std::function<void(Event)> _notify;
	void ShrinkToFit(); // optional optimization
	void Search(Tiles& soln, const Board& board, const Tiles& tiles);
	void LayTile(Board& newBoard, Tiles& newTiles, const Tiles& tiles, const Tile& choice) const;
	bool Intersect(const Tile& tile1, const Tile& tile2) const;
	void Subtract(Board& board, const Tile& tile) const;
	TI ChooseColumn(const Board& board) const;
	std::vector<TI> Covers(const TI col, const Tiles& tiles) const;
public:
	ACX() {}
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	ACX&operator=(const ACX&) = delete; // no assignment
	ACX(const ACX&) = delete; // no copy constructor
};
