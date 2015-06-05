// read data and run solver
// Bill Ola Rasmussen

#include "dlx.h"
#include "dlx2.h"
#include "dix.h"
#include "acx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <ctime>
#include <chrono>
#include <memory>
using namespace std;

// observer pattern
void CallBack(const Solver::Event e)
{
	static chrono::high_resolution_clock::time_point begin;
	static bool first(true);
	static int count(0);
	
	if(e==Solver::Event::Begin)
	{
		begin=chrono::high_resolution_clock::now();
		cout<<"event: begin\n";
	}
	if(e==Solver::Event::Soln)
	{
		++count;
		if(first)
		{
			const auto soln(chrono::high_resolution_clock::now());
			first=false;
			const auto d(chrono::duration_cast<chrono::milliseconds>(soln-begin));
			cout<<"time to first solution: "<<d.count()/1000.<<" seconds\n";
		}
	}
	if(e==Solver::Event::End)
	{
		const auto end(chrono::high_resolution_clock::now());
		const auto d(chrono::duration_cast<chrono::milliseconds>(end-begin));
		cout<<count<<" solution(s) found, full solve time: "<<d.count()/1000.<<" seconds\n";
	}
}

void readInput(Solver&solver, istream&is)
{
	string str;
	{ // read constraint counts (columns)
		while (is.good())
		{
			getline(is, str);
			if (str.length() && '[' == str[0]) { break; }
		}
		getline(is, str);
		stringstream ss(str);
		int pri, sec;
		ss >> pri >> sec;
		if (ss.fail()) { throw(runtime_error("fail reading constraint counts")); }
		cout << "primary constraints: " << pri << ", "
			<< "secondary constraints: " << sec << '\n';

		solver.Init(pri, sec);
	}
	while(is.good()) // read lines (rows)
	{
		getline(is,str);
		if(str.length()&&str[0]==']')
		{
			// while (is.good()) { getline(is, str); } // empty stream
			return;
		}
		stringstream ss(str);
		for(bool first(true);!ss.eof();first=false) // read numbers (columns)
		{
			int col;
			ss>>col;
			if(ss.bad()||ss.fail()){break;} // eof is ok
			first?solver.Row(col):solver.Col(col);
		}
	}
	throw(runtime_error("no closing bracket in input"));
}

int main(int argc, char *argv[])
{
	// 1st parameter: -q for quiet
	string flag(argc > 1 ? argv[1] : "");
	flag.resize(2);
	const bool quiet("-q" == flag);

	// 2nd parameter: name of input file, blank or cin for cin
	const string inputsrc(argc > 2 ? argv[2] : "cin");

	// 3rd parameter: solver name, blank for default dlx
	const string solverName(argc > 3 ? argv[3] : "dlx");

	cout << "Exact Cover Solver\n";
	try
	{
		unique_ptr<Solver> solver;
		if (solverName == "dlx") { solver.reset(new DLX()); }
		else if (solverName == "dlx2") { solver.reset(new DLX2()); }
		else if (solverName == "dix") { solver.reset(new DIX()); }
		else if (solverName == "acx") { solver.reset(new ACX()); }
		else { throw runtime_error("unknown solver: " + solverName); }

		cout << "reading input from " << inputsrc << "...\n";
		if ("cin" == inputsrc)
		{
			readInput(*solver, cin);
		}
		else // input from file
		{
			filebuf fb;
			fb.open(inputsrc, ios::in);
			istream is(&fb);
			readInput(*solver, is);
			fb.close();
		}

		solver->Solve(!quiet, CallBack);
	}
	catch (exception const&e)
	{
		cout << "Exception: " << e.what() << '\n';
	}
	cout << "done.\n";

	return 0;
}
