// read data and run solver
// Bill Ola Rasmussen

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include "dlx.h"
using namespace std;

unique_ptr<Solver> readInput()
{
	string str;
	while(cin.good())
	{
		getline(cin,str);
  		if(str.length()&&'['==str[0]){break;}
	}

	getline(cin,str);
  	stringstream ss(str);
  	int pri,sec;
  	ss>>pri>>sec;
  	if(ss.fail()){throw(runtime_error("fail reading constraint counts"));}
  	cout<<"primary constraints: "<<pri<<", "
  		<<"secondary constraints: "<<sec<<'\n';
  		
  	// todo: choose a solver
  	unique_ptr<Solver>solver(new DLX(pri,sec));

	while(cin.good())
	{
		getline(cin,str);
		stringstream ss(str);
		
  	
  	
//  	cout<<str<<' '<<str.length()<<'\n';
	// cout<< read n constraints todo
	}
	return solver;
}

int main()
{
	cout<<"Generalized Exact Cover Solver\n"
		<<"reading input...\n";
	try
	{
		unique_ptr<Solver>solver(readInput());
	}
	catch(exception const&e)
	{
		cout<<"Exception: "<<e.what()<<'\n';
	}		
	cout<<"done.\n";
	 		
	return(0);
}
