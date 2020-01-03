all:
	gcc main1.c -o test1 -lmyspisok -L.
	gcc main2.c -o test2 -ldl
lib:
	gcc spisok.c -fPIC -c
	gcc -shared -Wl,-soname,libmyspisok.so.1 -o libmyspisok.so.1.0 spisok.o
	ldconfig -n .
	ln -sf libmyspisok.so.1 libmyspisok.so
