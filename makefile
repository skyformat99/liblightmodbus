#This is really basic makefile for compiling modlib library
#In linking process you should link modlib.o to your project, as well as slave.o or master.o to gain master or slave functionality
#To get full functionality you can use modlib-full.o files

#By making target 'avr' or 'avr-debug' you can compile library for AVR devices
#You can specify more settings in AVR section below

CC = gcc
CFLAGS =

LD = ld
LDFLAGS =

MASTERFLAGS =
SLAVEFLAGS =

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

check: MASTERFLAGS += -DMODBUS_MASTER_REGISTERS=1
check: SLAVEFLAGS += -DMODBUS_SLAVE_REGISTERS=1
check: FORCE
	-mkdir test/modlib
	rsync -av --exclude test --exclude makefile --exclude .git --exclude .travis.yml \
	--exclude .gitattributes --exclude .gitignore --exclude README.md --exclude LICENSE . test/modlib
	cd test && $(CC) $(CFLAGS) --coverage -c modlib/master/mregisters.c
	cd test && $(CC) $(CFLAGS) --coverage -c modlib/slave/sregisters.c
	cd test && $(CC) $(CFLAGS) $(MASTERFLAGS) --coverage -c modlib/master.c
	cd test && $(CC) $(CFLAGS) $(SLAVEFLAGS) --coverage -c modlib/slave.c
	cd test && $(CC) $(CFLAGS) --coverage -c modlib/modlib.c
	cd test && $(CC) $(CFLAGS) --coverage -c test.c
	cd test && $(CC) $(CFLAGS) --coverage test.o modlib.o master.o slave.o mregisters.o sregisters.o -o test

run:
	cd test && ./test

debug: obj/modlib.o master-registers slave-registers FORCE #Same as 'all', without removing temp files


#### Modlib
obj/modlib.o: modlib.c modlib.h FORCE
	$(CC) $(CFLAGS) -c modlib.c -o obj/modlib.o
	echo "modlib.o: full" >> obj/versions.txt


#### Master
master-base: master.c master.h parser.h master/mtypes.h FORCE
	$(CC) $(CFLAGS) -c master.c -o obj/master-base.o
	mv obj/master-base.o obj/master.o
	echo "master.o: base" >> obj/versions.txt

master-registers: MASTERFLAGS += -DMODBUS_MASTER_REGISTERS=1
master-registers: master.c master.h parser.h master/mtypes.h master/mregisters.c master/mregisters.h FORCE
	$(CC) $(CFLAGS) $(MASTERFLAGS) -c master.c -o obj/master-base.o
	$(CC) $(CFLAGS) -c master/mregisters.c -o obj/master/mregisters.o
	$(LD) $(LDFLAGS) -r obj/master-base.o obj/master/mregisters.o -o obj/master-registers.o
	rm -rf obj/master-base.o
	mv obj/master-registers.o obj/master.o
	echo "master.o: registers" >> obj/versions.txt


#### Slave
slave-base: slave.c slave.h parser.h slave/stypes.h FORCE
	$(CC) $(CFLAGS) -c slave.c -o obj/slave-base.o
	mv obj/slave-base.o obj/slave.o
	echo "slave.o: base" >> obj/versions.txt

slave-registers: SLAVEFLAGS += -DMODBUS_SLAVE_REGISTERS=1
slave-registers: slave.c slave.h parser.h slave/stypes.h slave/sregisters.c slave/sregisters.h FORCE
	$(CC) $(CFLAGS) $(SLAVEFLAGS) -c slave.c -o obj/slave-base.o
	$(CC) $(CFLAGS) -c slave/sregisters.c -o obj/slave/sregisters.o
	$(LD) $(LDFLAGS) -r obj/slave-base.o obj/slave/sregisters.o -o obj/slave-registers.o
	rm -rf obj/slave-base.o
	mv obj/slave-registers.o obj/slave.o
	echo "slave.o: registers" >> obj/versions.txt


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
