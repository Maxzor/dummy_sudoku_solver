sudoku : sudoku.o mat_string.o
	cc -o sudoku sudoku.o mat_string.o

sudoku.o : sudoku.c sudoku.h mat_string.h
	cc -c sudoku.c

mat_string.o : mat_string.c mat_string.h
	cc -c mat_string.c

clean :
	rm -f sudoku *.o

