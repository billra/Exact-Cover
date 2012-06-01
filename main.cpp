// read data and run solver
// Bill Ola Rasmussen

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include "dlx.h"
using namespace std;

void streamState(ios&s)
{
    cout<<"good "<<s.good()<<", bad "<<s.bad()
        <<", fail "<<s.fail()<<", eof "<<s.eof()<<endl;
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

    solver.Init(pri,sec);    

    while(cin.good())
    {
        getline(cin,str);
        cout<<"got line: "<<str<<endl;
        streamState(cin);
        stringstream ss(str);
        
        // todo: exit on ']'
        string s{ss.str()};
        if(s.length()&&s[0]==']'){return;}
        
        bool initRow(true);
        while(true)
        {
            int col;
            cout<<" before ss into col";
            streamState(ss);
            ss>>col;
            cout<<" after ss into col";
            streamState(ss);
            if(ss.bad()||ss.fail()){break;} // eof is ok
            cout<<"  got int: "<<col<<endl;
            if(initRow)
            {
                cout<<"    new row with col: "<<col<<endl;
                solver.Row(col);
                initRow=false;
        
            }
            else
            {
                cout<<"    new col: "<<col<<endl;
                solver.Col(col);
            }
            if(ss.eof()){break;}
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
    }
    catch(exception const&e)
    {
        cout<<"Exception: "<<e.what()<<'\n';
    }        
    cout<<"done.\n";

    return(0);
}
