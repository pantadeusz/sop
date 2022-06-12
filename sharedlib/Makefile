
all: bin/libdeco.so

bin/libdeco.so: src/deco.o
	mkdir -p bin
	gcc -o bin/libdeco.so -shared src/deco.o
	
src/deco.o: src/deco.c
	gcc -Iinclude -c src/deco.c -fpic -Wall -Werror
	mv deco.o src/deco.o

install: bin/libdeco.so
	mkdir -p /usr/lib
	cp bin/libdeco.so /usr/lib/
	cp include/deco.h /usr/include/
	echo '/usr/lib' >  /etc/ld.so.conf.d/deco.conf
	ldconfig

uninstall: 	
	rm /usr/lib/libdeco.so
	rm /usr/include/deco.h
	rm /etc/ld.so.conf.d/deco.conf
	ldconfig

clean:
	rm -f src/*.o
	rm -f bin/libdeco.so
	
