// classic dancing links solver implementation
// Bill Ola Rasmussen
#include "dlx.h"

// Knuth's Algorithm X  from http://www-cs-faculty.stanford.edu/~uno/papers/dancing-color.ps.gz
// -------------------
// If A is empty, the problem is solved; terminate successfully.
// Otherwise choose a column, c (deterministically).
// Choose a row, r, such that A[r,c] = 1 (nondeterministically).
// Include r in the partial solution.
// For each j such that A[r,j] = 1,
//     delete column j from matrix A;
//         for each i such that A[i,j] = 1,
//             delete row i from matrix A.
// Repeat this algorithm recursively on the reduced matrix A.
//
// deterministically: choose column with fewest 1's

// Algorithm X dlx data structure (from same paper)
// ------------------------------
// One good way to implement algorithm X is to represent each 1 in the
// matrix A as a data object x with five fields L[x], R[x], U[x], D[x], C[x]. Rows of the matrix
// are doubly linked as circular lists via the L and R fields (“left” and “right”); columns are
// doubly linked as circular lists via the U and D fields (“up” and “down”). Each column
// list also includes a special data object called its list header.
//     The list headers are part of a larger object called a column object. Each column object
// y contains the fields L[y], R[y], U [y], D[y], and C[y] of a data object and two additional
// fields, S[y] (“size”) and N [y] (“name”); the size is the number of 1s in the column, and the
// name is a symbolic identifier for printing the answers. The C field of each object points
// to the column object at the head of the relevant column.
//     The L and R fields of the list headers link together all columns that still need to be
// covered. This circular list also includes a special column object called the root, h, which
// serves as a master header for all the active headers. The fields U[h], D[h], C[h], S[h], and
// N[h] are not used.

// Generalized Exact Cover (from same paper)
// -----------------------
// We can handle this extra complication by generalizing the exact cover problem. Instead
// of requiring all columns of a given 0-1 matrix to be covered by disjoint rows, we
// will distinguish two kinds of columns: primary and secondary. The generalized problem
// asks for a set of rows that covers every primary column exactly once and every secondary
// column at most once.
// 
// Fortunately, we can solve the generalized cover problem by using almost the same
// algorithm as before. The only difference is that we initialize the data structure by making
// a circular list of the column headers for the primary columns only. The header for each
// secondary column should have L and R fields that simply point to itself. The remainder
// of the algorithm proceeds exactly as before, so we will still call it algorithm DLX.


DLX::DLX(int pc, int sc):Solver(pc,sc)
{
}
DLX::~DLX()
{
}
void DLX::Row()
{
}
void DLX::Col(int column)
{
}
void DLX::Solve()
{
}

