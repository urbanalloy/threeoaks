TIMESTAMP = `date +%Y%m%d`
ZIPFILE = proxy-libintl-$(TIMESTAMP).zip

CC = gcc
CFLAGS = -Wall -I ../../include

X64CC = x86_64-pc-mingw32-gcc

all : ../../lib/libintl.a ../../lib/intl.lib ../../lib64/libintl.a ../../lib64/intl.lib

../../lib/libintl.a : libintl.o
	ar rc $@ libintl.o

../../lib/intl.lib : ../../lib/libintl.a
	cp ../../lib/libintl.a $@
	strip --strip-unneeded $@

../../lib64/libintl.a : libintl.x64.o
	x86_64-pc-mingw32-ar rc $@ libintl.x64.o

../../lib64/intl.lib : ../../lib64/libintl.a
	cp ../../lib64/libintl.a $@
	x86_64-pc-mingw32-strip --strip-unneeded $@

libintl.o : libintl.c
	$(CC) $(CFLAGS) -c libintl.c

libintl.x64.o : libintl.c
	$(X64CC) $(CFLAGS) -c -o $@ libintl.c

clean :
	rm -f *.o ../../lib/* ../../lib64/* ../../$(ZIPFILE)

dist : clean all
	cd ../..; zip $(ZIPFILE) include/libintl.h lib{,64}/{libintl.a,intl.lib} src/proxy-libintl/{README.txt,COPYING.LIB.txt,Makefile,libintl.[ch]}; manifestify $(ZIPFILE)
