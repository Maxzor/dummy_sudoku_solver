sudo : sudo.c sudo.h
	cc -o sudo sudo.c
	# cc -DTRACE -o sudo sudo.c

clean :
	rm -f sudo

