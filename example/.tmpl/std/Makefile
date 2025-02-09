all: libconvert.so libio.so

libconvert.so: native/convert.c
	gcc -shared -o libconvert.so -fPIC native/convert.c

libio.so: native/io.c
	gcc -shared -o libio.so -fPIC native/io.c