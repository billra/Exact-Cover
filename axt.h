// algorithm xt solver interface
// Bill Ola Rasmussen

#include "Solver.h"
#include <vector>

using VUI = std::vector<unsigned int>;
using VVUI = std::vector<VUI>;

struct Tile {
	Tile * prevTile; // dance Tile list
	Tile * nextTile;
	struct Conflict {
		Tile * prevConflict; // dance Conflict list
		Tile * nextConflict;
	};
	Conflict * conflict;
	unsigned int nConflict;
	std::vector<Conflict> conflictStorage; // stored here but manipulated via dance
	Tile() = default;
	Tile&operator=(const Tile&) = delete; // no assignment
	Tile(const Tile&) = delete; // no copy constructor
};

class Tiles {
	// Storage is pre-allocated in the constructor using Tile default constructor.
	// We have a guarantee that memory will not move under resize reallocation
	// as that code will not compile due to Tile's deleted copy constructor.
	std::vector<Tile> tileStorage; // stored here but manipulated via dance
public:
	Tiles(const VVUI& vtile); // linkup Tiles from initialization storage
	Tiles&operator=(const Tiles&) = delete; // no assignment
	Tiles(const Tiles&) = delete; // no copy constructor
};

class AXT :public Solver {
	std::function<void(Event)>Notify;
	bool show;
	unsigned int npc; // number of primary constraints
	VUI constraint;
	VVUI vtile;
	decltype(vtile.end()) itCurTile;
public:
	AXT() = default;
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	AXT&operator=(const AXT&) = delete; // no assignment
	AXT(const AXT&) = delete; // no copy constructor
};
