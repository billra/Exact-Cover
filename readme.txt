Code to create and solve exact cover problems.

----------
Motivation
----------
"Due to its NP-completeness, any problem in NP can be reduced to exact cover problems, which then can be solved with techniques such as Dancing Links. However, for some well known problems, the reduction is particularly direct. For instance, the problem of tiling a board with pentominoes, and solving Sudoku can both be viewed as exact cover problems." from http://en.wikipedia.org/wiki/Exact_cover#Noteworthy_examples

So, an exact cover solver can be used to solve any problem in NP.

-----------
Description
-----------

This project contains code to create exact cover data from problems:
CreateQueensData.py - n Queens problem
CreateYpentominoData.py - Y Pentomino cover
and a classic Algorithm X dancing links solver.

-----
Usage
-----
CreateQueensData {size} : create exact cover problem data for n queens board size
CreateQueensData : no parameter to run unit tests on creation code
CreateQueensData {size} | solve > soln.txt : stream problem into solver and write solution to file
e.g. CreateQueensData.py 6 | solve : prints the 4 solutions to a 6x6 board
     CreateQueensData.py 14 | solve -quiet : prints the number of solutions to a 14x14 board

CreateYpentominoData.py 11 20 | solve -quiet : count solutions to a 11x20 board
CreateYpentominoData.py 15 | solve : show solutions to a 15x15 board

Create png of the first solution:
CreateYpentominoData.py 10 14 | solve | boardPng.py 10 14

