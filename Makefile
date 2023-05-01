LIBMASTIK = -I./Mastik/include -L./Mastik/lib -l:libmastik.a

all: send recv

send:
	gcc -g -O2 send.c -o send $(LIBMASTIK)

recv:
	gcc -g -O2 recv.c -o recv $(LIBMASTIK)

clean:
	rm send recv
