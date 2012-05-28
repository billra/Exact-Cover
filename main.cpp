// read data and run solver
// Bill Ola Rasmussen

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;

void readInput()
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
  
	while(cin.good())
	{
		getline(cin,str);
		stringstream ss(str);
		
  	
  	
//  	cout<<str<<' '<<str.length()<<'\n';
	// cout<< read n constraints todo
	}
}

// interface:
//   ctor(pri,sec)
//   row()	// start a new row
//   col(colno) // item on a row
//   solve()

int main()
{
	cout<<"Generalized Exact Cover Solver\n"
		<<"reading input...\n";
	try
	{
		readInput();
	}
	catch(exception const&e)
	{
		cout<<"Exception: "<<e.what()<<'\n';
	}		
	cout<<"done.\n";
	 		
	return(0);
}
