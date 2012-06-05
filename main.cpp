// read data and run solver
// Bill Ola Rasmussen

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include "dlx.h"
using namespace std;

// observer pattern
void CallBack(Solver::Event e)
{
    // todo: report timing
    cout<<"event: ";
    std::string s{"unknown"};
    if(e==Solver::Event::Begin){s="Begin";}
    if(e==Solver::Event::Soln ){s="Soln";}
    if(e==Solver::Event::End  ){s="End";}
    cout<<s<<endl;
}

void readInput(Solver&solver)
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

    solver.Init(pri,sec,CallBack);    

    while(cin.good()) // read lines (rows)
    {
        getline(cin,str);
        if(str.length()&&str[0]==']'){return;}
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

int main()
{
    cout<<"Generalized Exact Cover Solver\n"
        <<"reading input...\n";
    try
    {
        // todo: choose a solver
        DLX solver;
        readInput(solver);
        solver.Solve();
    }
    catch(exception const&e)
    {
        cout<<"Exception: "<<e.what()<<'\n';
    }        
    cout<<"done.\n";

    return(0);
}
