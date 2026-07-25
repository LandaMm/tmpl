all: libio.so convert/libdouble.so convert/libfloat.so convert/libint.so convert/libstring.so

convert/libdouble.so: convert/double.c
	gcc -shared -o convert/libdouble.so -fPIC convert/double.c

convert/libfloat.so: convert/float.c
	gcc -shared -o convert/libfloat.so -fPIC convert/float.c

convert/libint.so: convert/int.c
	gcc -shared -o convert/libint.so -fPIC convert/int.c

convert/libstring.so: convert/string.c
	gcc -shared -o convert/libstring.so -fPIC convert/string.c

libio.so: native/io.c
	gcc -shared -o libio.so -fPIC native/io.c