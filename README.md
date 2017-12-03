# dummy_sudoku_solver
 A simple C program to solve classic 9*9 sudokus. It uses the method of single position candidate and greedy exploration, with eventually agressive bruteforce (50% guess) and backtracking. 

All credit goes to Patrick Foubet, teacher at the french FLOSS People's university "l'Ecole du logiciel libre", located in Ivry, in the Parisian suburb. https://www.e2li.org/
This program was developed as part of a lesson on advanced c programming, between end of 2016 and 2017.
There was done 90% of the codebase, I only translated the code to full English and expanded the algorithm to check for candidates in all subsets.

****Compiling****

This program should compile easily on most GNU/Linux or UNIX systems with `make sudoku`.
If you run Windows or Apple, do not run Windows or Apple ; to compile, get yourself sorted : use minGW/Cygwin, fire yourself up a WSL, a VM.

****Running this solver****

`./sudoku name_of_your_mat.txt`
Your grid should be in 'dirmat' directory. See examples for formatting. You can prefix the `time` command to get a measure of the solving duration.


****Solving algorithm****

The first part of the function `solve` iterates through values 1 to 9 and the three subsets : row, column, sub-matrix (block). In each subset, it will look for position candidates : if only one option is found - this means we are sure that the value goes here, we fill the value in the cell.

If the first part of the loop did not find any candidate (Ok flag is down), the second loop does the same iteration. But, when it finds a subset with two options, it appends a new matrix to the chaining structure, and fills the two options : one in the existing matrix, the other in the matrix just created. The two matrices are linked, and the algorithm loops back to the first part, exploring the last matrix created. If this matrix proves unsolvable this means the guess was wrong : it erases this matrix and rolls back to the previous one to continue exploration.

On exit the algorithm outputs the number of branches made.

****Various solving methods not implemented here****

This program resorts to the easiest solving method, the single position candidate. There are numerous other solving methods, see :

- https://www.sudokuoftheday.com/techniques/
- http://www.sudokuwiki.org/sudoku.htm

****Sudoku theory****

There is little theory about sudoku.

Sudoku theory relies on the following concepts :

- Grid topology - size, form
- Rules (classic, windowku...)
- Solving methods
- Number of givens
- Topology of givens

One team proved in recent years that the minimum amount of givens for a uniquely solvable classic 9*9 sudoku is 17.
https://arxiv.org/PS_cache/arxiv/pdf/1201/1201.0749v1.pdf. It did so by exploring the whole sudoku space with a super computer ; a mathematic desmonstration is yet to be proposed.

See also :

- https://en.wikipedia.org/wiki/Mathematics_of_Sudoku
- https://en.wikipedia.org/wiki/Sudoku_solving_algorithms
- white paper on a sudoku maker : https://sites.math.washington.edu/~morrow/mcm/team2280.pdf
