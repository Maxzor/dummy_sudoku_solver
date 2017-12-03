sudo : sudo.o chaine.o
	cc -o sudo sudo.o chaine.o

sudo.o : sudo.c sudo.h chaine.h
	cc -c sudo.c

chaine.o : chaine.c chaine.h
	cc -c chaine.c

clean :
	rm -f sudo *.o

