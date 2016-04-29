# This is makefile for Modlib - a lightweight Modbus library
# It creates object files and a static library, so you can link Modlib to your project
# If you want, Modlib can be installed on your system using 'make install', or later removed using 'make uninstall'
# If you want to build Modlib for platform like AVR, this is not the makefile you are looking for
# Beside, files building order is cruicial - modules first, then base, and finally linking, otherwise things will go wrong

# This makefile will be part of source deb package

CC = gcc
CFLAGS =

LD = ld
LDFLAGS =

MASTERFLAGS =
SLAVEFLAGS =

all: master-registers master-coils master-discrete-inputs master-input-registers master-base master-link
all: slave-registers slave-coils slave-discrete-inputs slave-input-registers slave-base slave-link
all: clean FORCE modlib-base
	$(LD) $(LDFLAGS) -r obj/modlib.o obj/master.o obj/slave.o -o obj/modlib-full.o
	echo "modlib-full.o: modlib.o, master.o and slave.o linked together" >> obj/versions.txt
	ar -cvq lib/libmodlib.a obj/modlib-full.o
	ar -t  lib/libmodlib.a

install:
	-mkdir -p $(DESTDIR)/usr
	-mkdir -p $(DESTDIR)/usr/include
	cp -R include/modlib $(DESTDIR)/usr/include
	-mkdir $(DESTDIR)/usr/lib
	cp -R lib/libmodlib.a $(DESTDIR)/usr/lib

uninstall:
	rm -rf $(DESTDIR)/usr/include/modlib
	rm -rf $(DESTDIR)/usr/lib/libmodlib.a

FORCE:
	-mkdir obj
	-mkdir obj/slave
	-mkdir obj/master
	-mkdir lib

clean:
	-rm -rf obj
	-rm -rf lib

################################################################################

modlib-base: src/modlib.c include/modlib/modlib.h
	$(CC) $(CFLAGS) -c src/modlib.c -o obj/modlib.o
	echo "modlib.o: full" >> obj/versions.txt

master-base: src/master.c include/modlib/master.h
	$(CC) $(CFLAGS) $(MASTERFLAGS) -c src/master.c -o obj/master/mbase.o
	echo "master.o: base" >> obj/versions.txt

master-registers: MASTERFLAGS += -DMODBUS_MASTER_REGISTERS=1
master-registers: src/master/mregisters.c include/modlib/master/mregisters.h
	$(CC) $(CFLAGS) -c src/master/mregisters.c -o obj/master/mregisters.o
	echo "master.o: registers" >> obj/versions.txt

master-coils: MASTERFLAGS += -DMODBUS_MASTER_COILS=1
master-coils: src/master/mcoils.c include/modlib/master/mcoils.h
	$(CC) $(CFLAGS) -c src/master/mcoils.c -o obj/master/mcoils.o
	echo "master.o: coils" >> obj/versions.txt

master-discrete-inputs: MASTERFLAGS += -DMODBUS_MASTER_DISCRETE_INPUTS=1
master-discrete-inputs: src/master/mdiscreteinputs.c include/modlib/master/mdiscreteinputs.h
	$(CC) $(CFLAGS) -c src/master/mdiscreteinputs.c -o obj/master/mdiscreteinputs.o
	echo "master.o: discrete inputs" >> obj/versions.txt

master-input-registers: MASTERFLAGS += -DMODBUS_MASTER_INPUT_REGISTERS=1
master-input-registers: src/master/minputregisters.c include/modlib/master/minputregisters.h
	$(CC) $(CFLAGS) -c src/master/minputregisters.c -o obj/master/minputregisters.o
	echo "master.o: input registers" >> obj/versions.txt

master-link:
	$(LD) $(LDFLAGS) -r obj/master/*.o -o obj/master.o
	echo "master.o: ~linked~" >> obj/versions.txt

slave-base: src/slave.c include/modlib/slave.h
	$(CC) $(CFLAGS) $(SLAVEFLAGS) -c src/slave.c -o obj/slave/sbase.o
	echo "slave.o: base" >> obj/versions.txt

slave-registers: SLAVEFLAGS += -DMODBUS_SLAVE_REGISTERS=1
slave-registers: src/slave/sregisters.c include/modlib/slave/sregisters.h
	$(CC) $(CFLAGS) -c src/slave/sregisters.c -o obj/slave/sregisters.o
	echo "slave.o: registers" >> obj/versions.txt

slave-coils: SLAVEFLAGS += -DMODBUS_SLAVE_COILS=1
slave-coils: src/slave/scoils.c include/modlib/slave/scoils.h
	$(CC) $(CFLAGS) -c src/slave/scoils.c -o obj/slave/scoils.o
	echo "slave.o: coils" >> obj/versions.txt

slave-discrete-inputs: SLAVEFLAGS += DMODBUS_SLAVE_DISCRETE_INPUTS=1
slave-discrete-inputs: src/slave/sdiscreteinputs.c include/modlib/slave/sdiscreteinputs.h
	$(CC) $(CFLAGS) -c src/slave/sdiscreteinputs.c -o obj/slave/sdiscreteinputs.o
	echo "slave.o: discrete inputs" >> obj/versions.txt

slave-input-registers: SLAVEFLAGS += DMODBUS_SLAVE_INPUT_REGISTERS=1
slave-input-registers: src/slave/sinputregisters.c include/modlib/slave/sinputregisters.h
	$(CC) $(CFLAGS) -c src/slave/sinputregisters.c -o obj/slave/sinputregisters.o
	echo "slave.o: input registers" >> obj/versions.txt

slave-link:
	$(LD) $(LDFLAGS) -r obj/slave/*.o -o obj/slave.o
	echo "slave.o: ~linked~" >> obj/versions.txt
