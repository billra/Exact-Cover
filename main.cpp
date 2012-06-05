// read data and run solver
// Bill Ola Rasmussen

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <ctime>
#include <chrono>
#include "dlx.h"
using namespace std;

// observer pattern
void CallBack(Solver::Event e)
{
    static chrono::high_resolution_clock::time_point begin,soln,end;
    static bool first{true};
    static int count{0};
    
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
            soln=chrono::high_resolution_clock::now();
            first=false;
            auto d = chrono::duration_cast<chrono::milliseconds>(soln-begin);
            cout<<"time to first solution: "<<d.count()/1000.<<" seconds\n";
        }
    }
    if(e==Solver::Event::End)
    {
        end=chrono::high_resolution_clock::now();
        auto d = chrono::duration_cast<chrono::milliseconds>(end-begin);
        cout<<count<<" solution(s) found, full solve time: "<<d.count()/1000.<<" seconds\n";
    }
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

int main(int argc, char *argv[])
{
    string flag(argc>1?argv[1]:"");
    flag.resize(2);
    const bool quiet{"-q"==flag};
    
    cout<<"Generalized Exact Cover Solver\n"
        <<"reading input...\n";
    try
    {
        // todo: choose a solver
        DLX solver;
        readInput(solver);
        solver.Solve(!quiet);
    }
    catch(exception const&e)
    {
        cout<<"Exception: "<<e.what()<<'\n';
    }        
    cout<<"done.\n";

    return(0);
}
