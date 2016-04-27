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

coverage: MASTERFLAGS = -DMODBUS_MASTER_REGISTERS=1 -DMODBUS_MASTER_COILS=1 -DMODBUS_MASTER_DISCRETE_INPUTS=1 -DMODBUS_MASTER_INPUT_REGISTERS=1
coverage: SLAVEFLAGS = -DMODBUS_SLAVE_REGISTERS=1 -DMODBUS_SLAVE_COILS=1 -DMODBUS_SLAVE_DISCRETE_INPUTS=1 -DMODBUS_SLAVE_INPUT_REGISTERS=1
coverage: CFLAGS += --coverage
coverage: FORCE
	-mkdir test/modlib
	rsync -av --exclude test --exclude makefile --exclude .git --exclude .travis.yml \
	--exclude .gitattributes --exclude .gitignore --exclude README.md --exclude LICENSE . test/modlib
	cd test && $(CC) $(CFLAGS) -c modlib/master/mregisters.c
	cd test && $(CC) $(CFLAGS) -c modlib/slave/sregisters.c
	cd test && $(CC) $(CFLAGS) -c modlib/master/mcoils.c
	cd test && $(CC) $(CFLAGS) -c modlib/slave/scoils.c
	cd test && $(CC) $(CFLAGS) -c modlib/master/mdiscreteinputs.c
	cd test && $(CC) $(CFLAGS) -c modlib/slave/sdiscreteinputs.c
	cd test && $(CC) $(CFLAGS) -c modlib/master/minputregisters.c
	cd test && $(CC) $(CFLAGS) -c modlib/slave/sinputregisters.c
	cd test && $(CC) $(CFLAGS) $(MASTERFLAGS) -c modlib/master.c
	cd test && $(CC) $(CFLAGS) $(SLAVEFLAGS) -c modlib/slave.c
	cd test && $(CC) $(CFLAGS) -c modlib/modlib.c
	cd test && $(CC) $(CFLAGS) -c test.c
	cd test && $(CC) $(CFLAGS) test.o modlib.o master.o slave.o mregisters.o sregisters.o mcoils.o scoils.o mdiscreteinputs.o sdiscreteinputs.o sinputregisters.o minputregisters.o -o test

check: debug
	-mkdir test/modlib
	rsync -av --exclude test --exclude makefile --exclude .git --exclude .travis.yml \
	--exclude .gitattributes --exclude .gitignore --exclude README.md --exclude LICENSE . test/modlib
	$(CC) $(CFLAGS) -c test/test.c -o test/test.o
	$(CC) $(CFLAGS) test/test.o obj/modlib.o obj/master.o obj/slave.o -o test/test

run:
	cd test && ./test

debug: FORCE
debug: MASTERFLAGS += -DMODBUS_MASTER_REGISTERS=1 -DMODBUS_MASTER_COILS=1 -DMODBUS_MASTER_DISCRETE_INPUTS=1 -DMODBUS_MASTER_INPUT_REGISTERS=1
debug: SLAVEFLAGS += -DMODBUS_SLAVE_REGISTERS=1 -DMODBUS_SLAVE_COILS=1 -DMODBUS_SLAVE_DISCRETE_INPUTS=1 -DMODBUS_SLAVE_INPUT_REGISTERS=1
debug: modlib-base
debug: master-registers master-coils master-discrete-inputs master-input-registers master-base master-link
debug: slave-registers slave-coils slave-discrete-inputs slave-input-registers slave-base slave-link


#### Modlib
modlib-base: modlib.c modlib.h
	$(CC) $(CFLAGS) -c modlib.c -o obj/modlib.o
	echo "modlib.o: full" >> obj/versions.txt


#### Master
master-base: master.c master.h parser.h master/mtypes.h
	$(CC) $(CFLAGS) $(MASTERFLAGS) -c master.c -o obj/master/mbase.o
	echo "master.o: base" >> obj/versions.txt

master-registers: parser.h master/mtypes.h master/mregisters.c master/mregisters.h
	$(CC) $(CFLAGS) -c master/mregisters.c -o obj/master/mregisters.o
	echo "master.o: registers" >> obj/versions.txt

master-coils: parser.h master/mtypes.h master/mcoils.c master/mcoils.h
	$(CC) $(CFLAGS) -c master/mcoils.c -o obj/master/mcoils.o
	echo "master.o: coils" >> obj/versions.txt

master-discrete-inputs: parser.h master/mtypes.h master/mdiscreteinputs.c master/mdiscreteinputs.h
	$(CC) $(CFLAGS) -c master/mdiscreteinputs.c -o obj/master/mdiscreteinputs.o
	echo "master.o: discrete inputs" >> obj/versions.txt

master-input-registers: parser.h master/mtypes.h master/minputregisters.c master/minputregisters.h
	$(CC) $(CFLAGS) -c master/minputregisters.c -o obj/master/minputregisters.o
	echo "master.o: input registers" >> obj/versions.txt

master-link:
	$(LD) $(LDFLAGS) -r obj/master/*.o -o obj/master.o
	echo "master.o: ~linked~" >> obj/versions.txt



#### Slave
slave-base: slave.c slave.h parser.h slave/stypes.h
	$(CC) $(CFLAGS) $(SLAVEFLAGS) -c slave.c -o obj/slave/sbase.o
	echo "slave.o: base" >> obj/versions.txt

slave-registers: parser.h slave/stypes.h slave/sregisters.c slave/sregisters.h
	$(CC) $(CFLAGS) -c slave/sregisters.c -o obj/slave/sregisters.o
	echo "slave.o: registers" >> obj/versions.txt

slave-coils: parser.h slave/stypes.h slave/scoils.c slave/scoils.h
	$(CC) $(CFLAGS) -c slave/scoils.c -o obj/slave/scoils.o
	echo "slave.o: coils" >> obj/versions.txt

slave-discrete-inputs: parser.h slave/stypes.h slave/sdiscreteinputs.c slave/sdiscreteinputs.h
	$(CC) $(CFLAGS) -c slave/sdiscreteinputs.c -o obj/slave/sdiscreteinputs.o
	echo "slave.o: discrete inputs" >> obj/versions.txt

slave-input-registers: parser.h slave/stypes.h slave/sinputregisters.c slave/sinputregisters.h
	$(CC) $(CFLAGS) -c slave/sinputregisters.c -o obj/slave/sinputregisters.o
	echo "slave.o: input registers" >> obj/versions.txt

slave-link:
	$(LD) $(LDFLAGS) -r obj/slave/*.o -o obj/slave.o
	echo "slave.o: ~linked~" >> obj/versions.txt

#### Utilities
FORCE: clean
	-mkdir obj
	-mkdir obj/slave
	-mkdir obj/master

clean:
	-rm -rf obj
	-rm -rf *.o
	-rm -rf master/*.o
	-rm -rf slave/*.o
	-rm -rf test/modlib
	-rm -rf test/*.gcov
	-rm -rf test/*.gcno
	-rm -rf test/*.gcda
	-rm -rf test/*.o
	-rm -rf test/modlib
	-rm -rf test/test
