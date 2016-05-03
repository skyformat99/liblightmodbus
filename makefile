# This is makefile for Modlib - a lightweight Modbus library
# It creates object files and a static library, so you can link Modlib to your project
# If you want, Modlib can be installed on your system using 'make install', or later removed using 'make uninstall'
# If you want to build Modlib for platform like AVR, this is not the makefile you are looking for
# Beside, files building order is cruicial - modules first, then base, and finally linking, otherwise things will go wrong

# This makefile will be part of source deb package

CC = gcc
CFLAGS = -Wall

LD = ld
LDFLAGS =

MASTERFLAGS =
SLAVEFLAGS =

all: master-registers master-coils master-discrete-inputs master-input-registers master-base master-link
all: slave-registers slave-coils slave-discrete-inputs slave-input-registers slave-base slave-link
all: clean FORCE modlib-base
	echo "LINKING Modlib full object file (obj/modlib-full.o)" >> modlib.log
	$(LD) $(LDFLAGS) -r obj/base.o obj/master.o obj/slave.o -o obj/modlib-full.o
	echo "CREATING Modlib static library file (lib/libmodlib.a)" >> modlib.log
	ar -cvq lib/libmodlib.a obj/modlib-full.o
	ar -t  lib/libmodlib.a
	echo -n "\n\nBuild success - " >> modlib.log
	date >> modlib.log

install:
	-mkdir -p $(DESTDIR)/usr
	-mkdir -p $(DESTDIR)/usr/include
	cp -R include/modlib $(DESTDIR)/usr/include
	-mkdir $(DESTDIR)/usr/lib
	cp -R lib/libmodlib.a $(DESTDIR)/usr/lib

uninstall:
	rm -rf $(DESTDIR)/usr/include/modlib
	rm -f $(DESTDIR)/usr/lib/libmodlib.a

FORCE:
	-touch modlib.log
	echo -n "Architecture: " > modlib.log
	arch >> modlib.log
	echo -n "Build started - " >> modlib.log
	date >> modlib.log
	echo -n "\n\n" >> modlib.log
	-mkdir obj
	-mkdir obj/slave
	-mkdir obj/master
	-mkdir lib

clean:
	-rm -rf obj
	-rm -rf lib
	-rm -f modlib.log
	-rm -f *.gcno
	-rm -f *.gcda
	-rm -f *.o
	-rm -f coverage-test
	-rm -f coverage-test.log
	-rm -f valgrind.xml
	-rm -f massif.out

################################################################################

modlib-base: src/modlib.c include/modlib/modlib.h
	echo "COMPILING Modlib base (obj/base.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/modlib.c -o obj/base.o

master-base: src/master.c include/modlib/master.h
	echo "COMPILING Master module (obj/master/mbase.o)" >> modlib.log
	$(CC) $(CFLAGS) $(MASTERFLAGS) -c src/master.c -o obj/master/mbase.o

master-registers: MASTERFLAGS += -DMODBUS_MASTER_REGISTERS=1
master-registers: src/master/mregisters.c include/modlib/master/mregisters.h
	echo "COMPILING Master registers module (obj/master/mregisters.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/master/mregisters.c -o obj/master/mregisters.o

master-coils: MASTERFLAGS += -DMODBUS_MASTER_COILS=1
master-coils: src/master/mcoils.c include/modlib/master/mcoils.h
	echo "COMPILING Master coils module (obj/master/mcoils.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/master/mcoils.c -o obj/master/mcoils.o

master-discrete-inputs: MASTERFLAGS += -DMODBUS_MASTER_DISCRETE_INPUTS=1
master-discrete-inputs: src/master/mdiscreteinputs.c include/modlib/master/mdiscreteinputs.h
	echo "COMPILING Master discrete inputs module (obj/master/mdiscreteinputs.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/master/mdiscreteinputs.c -o obj/master/mdiscreteinputs.o

master-input-registers: MASTERFLAGS += -DMODBUS_MASTER_INPUT_REGISTERS=1
master-input-registers: src/master/minputregisters.c include/modlib/master/minputregisters.h
	echo "COMPILING Master input registers module (obj/master/minputregisters.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/master/minputregisters.c -o obj/master/minputregisters.o

master-link:
	echo "LINKING Master module (obj/master.o)" >> modlib.log
	$(LD) $(LDFLAGS) -r obj/master/*.o -o obj/master.o

slave-base: src/slave.c include/modlib/slave.h
	echo "COMPILING Slave module (obj/slave/sbase.o)" >> modlib.log
	$(CC) $(CFLAGS) $(SLAVEFLAGS) -c src/slave.c -o obj/slave/sbase.o

slave-registers: SLAVEFLAGS += -DMODBUS_SLAVE_REGISTERS=1
slave-registers: src/slave/sregisters.c include/modlib/slave/sregisters.h
	echo "COMPILING Slave registers module (obj/slave/sregisters.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/slave/sregisters.c -o obj/slave/sregisters.o

slave-coils: SLAVEFLAGS += -DMODBUS_SLAVE_COILS=1
slave-coils: src/slave/scoils.c include/modlib/slave/scoils.h
	echo "COMPILING Slave coils module (obj/slave/scoils.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/slave/scoils.c -o obj/slave/scoils.o

slave-discrete-inputs: SLAVEFLAGS += DMODBUS_SLAVE_DISCRETE_INPUTS=1
slave-discrete-inputs: src/slave/sdiscreteinputs.c include/modlib/slave/sdiscreteinputs.h
	echo "COMPILING Slave discrete inputs module (obj/slave/sdiscreteinputs.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/slave/sdiscreteinputs.c -o obj/slave/sdiscreteinputs.o

slave-input-registers: SLAVEFLAGS += DMODBUS_SLAVE_INPUT_REGISTERS=1
slave-input-registers: src/slave/sinputregisters.c include/modlib/slave/sinputregisters.h
	echo "COMPILING Slave input registers module (obj/slave/sinputregisters.o)" >> modlib.log
	$(CC) $(CFLAGS) -c src/slave/sinputregisters.c -o obj/slave/sinputregisters.o

slave-link:
	echo "LINKING Slave module (obj/slave.o)" >> modlib.log
	$(LD) $(LDFLAGS) -r obj/slave/*.o -o obj/slave.o
