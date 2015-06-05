// array copy solver
// motivation: sequential access is very fast on modern architectures
// Bill Ola Rasmussen

#include "Solver.h"

class ACX : public Solver {
	// C++11 <cstdint> types:
	//	uint8_t		0 to           255	1 byte
	//	int16_t		0 to        65,535	2 bytes
	//	int32_t		0 to 4,294,967,295	4 bytes, same as Microsoft unsigned int
	using TI = unsigned int; // index type
	bool _show;
	std::function<void(Event)> _notify;
public:
	ACX(){}
	void Init(const unsigned int pc, const unsigned int sc) override;
	void Row(const unsigned int col) override;
	void Col(const unsigned int col) override;
	void Solve(const bool showSoln, std::function<void(Event)>CallBack) override;
	ACX&operator=(const ACX&)=delete; // no assignment
	ACX(const ACX&)=delete; // no copy constructor
};
