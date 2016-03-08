#This is really basic makefile for compiling modlib library
#In linking process you should link modlib.o to your project, as well as slave.o or master.o to gain master or slave functionality 

modlib.o: modlib.c modlib.h
	gcc -c modlib.c

master.o: master.c master.h
	gcc -c master.c

slave.o: slave.c slave.h
	gcc -c slave.c
