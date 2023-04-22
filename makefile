CC=gcc
FLAGS=-Wall -g -o

all: cmp copy libcodecA.so libcodecB.so encode decode stshell

cmp: cmp.c
	$(CC) $(FLAGS) cmp cmp.c
copy: copy.c
	$(CC) $(FLAGS) copy copy.c
libcodecA.so: codecA.c
	$(CC) -shared -fPIC -o libcodecA.so codecA.c
libcodecB.so: codecB.c
	$(CC) -shared -fPIC -o libcodecB.so codecB.c
encode: encode.c headers.h
	$(CC) $(FLAGS) encode encode.c ./libcodecA.so ./libcodecB.so
decode: decode.c headers.h
	$(CC) $(FLAGS) decode decode.c ./libcodecA.so ./libcodecB.so
stshell: stshell.c
	$(CC) $(FLAGS) stshell stshell.c
clean:
	rm cmp copy encode decode stshell libcodecA.so libcodecB.so
