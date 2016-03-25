#This is really basic makefile for compiling modlib library
#In linking process you should link modlib.o to your project, as well as slave.o or master.o to gain master or slave functionality
#To get full functionality you can use modlib-full.o files

#By making target 'avr' or 'avr-debug' you can compile library for AVR devices
#You can specify more settings in AVR section below

CC = gcc
CFLAGS =

LD = ld
LDFLAGS =

#### AVR
MCU = atmega8
AVRCFLAGS = -mmcu=$(MCU)
AVRLDFLAGS =

avr: CC = avr-gcc
avr: LD = avr-ld
avr: CFLAGS = $(AVRCFLAGS)
avr: LDFLAGS = $(AVRLDFLAGS)
avr: all
	avr-size -C --mcu=$(MCU) obj/modlib.o
	avr-size -C --mcu=$(MCU) obj/master.o
	avr-size -C --mcu=$(MCU) obj/slave.o
	avr-size -C --mcu=$(MCU) obj/modlib-full.o
	sed -i "1i ---COMPLIED FOR AVR---" obj/versions.txt

avr-debug: CC = avr-gcc
avr-debug: LD = avr-ld
avr-debug: CFLAGS = $(AVRCFLAGS)
avr-debug: LDFLAGS = $(AVRLDFLAGS)
avr-debug: debug
	avr-size -C --mcu=$(MCU) obj/modlib.o
	avr-size -C --mcu=$(MCU) obj/master.o
	avr-size -C --mcu=$(MCU) obj/slave.o
	sed -i "1i ---COMPLIED FOR AVR---" obj/versions.txt


#### Main
all: debug FORCE #Same as 'debug' currently, but removes temporary files
	rm -rf obj/master
	rm -rf obj/slave
	$(LD) $(LDFLAGS) -r obj/modlib.o obj/master.o obj/slave.o -o obj/modlib-full.o
	echo "modlib-full.o: modlib.o, master.o and slave.o linked together" >> obj/versions.txt

check: FORCE
	rsync -av --exclude test --exclude makefile --exclude .git --exclude .travis.yml --exclude .gitattributes --exclude .gitignore --exclude README.md --exclude LICENSE ../modlib test
	mv test/modlib/master/basic.c test/modlib/master/mbasic.c
	mv test/modlib/slave/basic.c test/modlib/slave/sbasic.c
	cd test && $(CC) $(CFLAGS) --coverage -c modlib/master/mbasic.c
	cd test && $(CC) $(CFLAGS) --coverage -c modlib/slave/sbasic.c
	cd test && $(CC) $(CFLAGS) --coverage -DMODBUS_MASTER_BASIC=1 -c modlib/master.c
	cd test &&$(CC) $(CFLAGS) --coverage -DMODBUS_SLAVE_BASIC=1 -c modlib/slave.c
	cd test && $(CC) $(CFLAGS) --coverage -c modlib/modlib.c
	cd test && $(CC) $(CFLAGS) --coverage -c test.c
	cd test && $(CC) $(CFLAGS) --coverage test.o modlib.o master.o slave.o mbasic.o sbasic.o -o test
	cd test && ./test

debug: obj/modlib.o master-basic slave-basic FORCE #Same as 'all', without removing temp files


#### Modlib
obj/modlib.o: modlib.c modlib.h FORCE
	$(CC) $(CFLAGS) -c modlib.c -o obj/modlib.o
	echo "modlib.o: full" >> obj/versions.txt


#### Master
master-base: master.c master.h parser.h FORCE
	$(CC) $(CFLAGS) -c master.c -o obj/master-base.o
	mv obj/master-base.o obj/master.o
	echo "master.o: base" >> obj/versions.txt

master-basic: master.c master.h parser.h master/basic.c master/basic.h FORCE
	$(CC) $(CFLAGS) -c master.c -DMODBUS_MASTER_BASIC=1 -o obj/master-base.o
	$(CC) $(CFLAGS) -c master/basic.c -o obj/master/basic.o
	$(LD) $(LDFLAGS) -r obj/master-base.o obj/master/basic.o -o obj/master-basic.o
	rm -rf obj/master-base.o
	mv obj/master-basic.o obj/master.o
	echo "master.o: basic" >> obj/versions.txt


#### Slave
slave-base: slave.c slave.h parser.h slave/types.h FORCE
	$(CC) $(CFLAGS) -c slave.c -o obj/slave-base.o
	mv obj/slave-base.o obj/slave.o
	echo "slave.o: base" >> obj/versions.txt

slave-basic: slave.c slave.h parser.h slave/types.h slave/basic.c slave/basic.h FORCE
	$(CC) $(CFLAGS) -c slave.c -DMODBUS_SLAVE_BASIC=1 -o obj/slave-base.o
	$(CC) $(CFLAGS) -c slave/basic.c -o obj/slave/basic.o
	$(LD) $(LDFLAGS) -r obj/slave-base.o obj/slave/basic.o -o obj/slave-basic.o
	rm -rf obj/slave-base.o
	mv obj/slave-basic.o obj/slave.o
	echo "slave.o: basic" >> obj/versions.txt


#### Utilities
FORCE: clean
	-mkdir obj
	-mkdir obj/slave
	-mkdir obj/master

clean:
	-rm -rf obj
	-rm -rf test/modlib
	-rm -rf test/*.gcov
	-rm -rf test/*.gcno
	-rm -rf test/*.gcda
	-rm -rf test/*.o
	-rm -rf test/modlib
	-rm -rf test/test
