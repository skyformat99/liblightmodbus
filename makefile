#This is really basic makefile for compiling modlib library
#In linking process you should link modlib.o to your project, as well as slave.o or master.o to gain master or slave functionality

CC = gcc
LD = ld

all: obj/modlib.o master-full slave-full FORCE

#This target makes as many files as possible (because of development some files don't exist yet)
most: obj/modlib.o master-basic slave-basic FORCE

obj/modlib.o: modlib.c modlib.h FORCE
	$(CC) -c modlib.c -o obj/modlib.o


#### Master

master-base: master.c master.h parser.h FORCE
	$(CC) -c master.c -o obj/master-base.o

master-basic: master.c master.h parser.h master/basic.c master/basic.h FORCE
	$(CC) -c master.c -DMODBUS_MASTER_BASIC=1 -o obj/master-base.o
	$(CC) -c master/basic.c -o obj/master/basic.o
	$(LD) -r obj/master-base.o obj/master/basic.o -o obj/master-basic.o

master-full: master-basic master/full.c master/full.h
	$(CC) -c master/full.c -o obj/master/full.o
	$(LD) -r obj/master-basic.o obj/master/full.o -o obj/master-full.o

#### Slave

slave-base: slave.c slave.h parser.h slave/types.h FORCE
	$(CC) -c slave.c -o obj/slave-base.o

slave-basic: slave.c slave.h parser.h slave/types.h slave-base slave/basic.c slave/basic.h FORCE
	$(CC) -c slave.c -DMODBUS_SLAVE_BASIC=1 -o obj/slave-base.o
	$(CC) -c slave/basic.c -o obj/slave/basic.o
	$(LD) -r obj/slave-base.o obj/slave/basic.o -o obj/slave-basic.o

slave-full: slave-basic slave/full.c slave/full.h
	$(CC) -c slave/full.c -o obj/slave/full.o
	$(LD) -r obj/slave-basic.o obj/slave/full.o -o obj/slave-full.o

#### Utilities

clean:
	rm -rf obj

FORCE:
	mkdir obj
	mkdir obj/slave
	mkdir obj/master
