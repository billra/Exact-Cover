// algorithm xt solver interface
// Bill Ola Rasmussen

#include "Solver.h"
#include <vector>

using VVUI = std::vector<std::vector<unsigned int>>;

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
	std::vector<unsigned int> constraint;
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
