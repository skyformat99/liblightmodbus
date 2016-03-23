#This is really basic makefile for compiling modlib library
#In linking process you should link modlib.o to your project, as well as slave.o or master.o to gain master or slave functionality

CC = gcc
LD = ld

all: debug FORCE #Same as 'debug' currently, but removes temporary files
	rm -rf obj/master
	rm -rf obj/slave
	$(LD) -r obj/modlib.o obj/master.o obj/slave.o -o obj/modlib-full.o
	echo "modlib-full.o: modlib.o, master.o and slave.o linked together" >> obj/versions.txt

debug: obj/modlib.o master-basic slave-basic FORCE #Same as 'all', without removing temp files

obj/modlib.o: modlib.c modlib.h FORCE
	$(CC) -c modlib.c -o obj/modlib.o
	echo "modlib.o: full" >> obj/versions.txt


#### Master
master-base: master.c master.h parser.h FORCE
	$(CC) -c master.c -o obj/master-base.o
	mv obj/master-base.o obj/master.o
	echo "master.o: base" >> obj/versions.txt

master-basic: master.c master.h parser.h master/basic.c master/basic.h FORCE
	$(CC) -c master.c -DMODBUS_MASTER_BASIC=1 -o obj/master-base.o
	$(CC) -c master/basic.c -o obj/master/basic.o
	$(LD) -r obj/master-base.o obj/master/basic.o -o obj/master-basic.o
	rm -rf obj/master-base.o
	mv obj/master-basic.o obj/master.o
	echo "master.o: basic" >> obj/versions.txt


#### Slave
slave-base: slave.c slave.h parser.h slave/types.h FORCE
	$(CC) -c slave.c -o obj/slave-base.o
	mv obj/slave-base.o obj/slave.o
	echo "slave.o: base" >> obj/versions.txt

slave-basic: slave.c slave.h parser.h slave/types.h slave/basic.c slave/basic.h FORCE
	$(CC) -c slave.c -DMODBUS_SLAVE_BASIC=1 -o obj/slave-base.o
	$(CC) -c slave/basic.c -o obj/slave/basic.o
	$(LD) -r obj/slave-base.o obj/slave/basic.o -o obj/slave-basic.o
	rm -rf obj/slave-base.o
	mv obj/slave-basic.o obj/slave.o
	echo "slave.o: basic" >> obj/versions.txt

#### Utilities
FORCE: clean
	mkdir obj
	mkdir obj/slave
	mkdir obj/master

clean:
	rm -rf obj
