# This is makefile for liblightmodbus - a lightweight Modbus library
# It creates object files and a static library, so you can link it to your project
# If you want, liblightmodbus can be installed on your system using 'make install', or later removed using 'make uninstall'
# If you want to build it yourself for platform like AVR, this is not the makefile you are looking for
# Beside, files building order is cruicial - modules first, then base, and finally linking, otherwise things will go wrong

# This makefile is part of source deb package

compileHeader = \
	echo "[\033[32;1mcompiling\033[0m] \033[03m$(1)\033[0m" >&2

linkHeader = \
	echo "[\033[33;1mlinking\033[0m] \033[03m$(1)\033[0m" >&2

errorHeader = \
	echo "[\033[31;1merror\033[0m] \033[03m$(1)\033[0m" >&2

cleanHeader = \
	echo "[\033[36;1mcleaning up\033[0m] \033[03m$(1)\033[0m" >&2

infoHeader = \
	echo "[\033[34;1minfo\033[0m] \033[03m$(1)\033[0m" >&2

CC = gcc
CFLAGS = -Wall -I./include

LD = ld
LDFLAGS =

MASTERFLAGS =
SLAVEFLAGS =

all: master-registers master-coils master-discrete-inputs master-input-registers master-base master-link
all: slave-registers slave-coils slave-discrete-inputs slave-input-registers slave-base slave-link
all: clean FORCE core
	$(call linkHeader,full object file)
	echo "LINKING Library full object file (obj/lightmodbus.o)" >> build.log
	$(LD) $(LDFLAGS) -r obj/core.o obj/master.o obj/slave.o -o obj/lightmodbus.o
	$(call linkHeader,static library file)
	echo "CREATING Static library file (lib/liblightmodbus.a)" >> build.log
	ar -cvq lib/liblightmodbus.a obj/lightmodbus.o
	ar -t  lib/liblightmodbus.a
	echo -n "\n\nBuild success - " >> build.log
	date >> build.log
	$(call infoHeader,build finished successfully)

install:
	$(call infoHeader,installing liblightmodbus)
	-mkdir -p $(DESTDIR)/usr
	-mkdir -p $(DESTDIR)/usr/include
	cp -R include/lightmodbus $(DESTDIR)/usr/include
	-mkdir $(DESTDIR)/usr/lib
	cp -R lib/liblightmodbus.a $(DESTDIR)/usr/lib

uninstall:
	$(call infoHeader,removing liblightmodbus)
	rm -rf $(DESTDIR)/usr/include/lightmodbus
	rm -f $(DESTDIR)/usr/lib/liblightmodbus.a

FORCE:
	$(call infoHeader,starting build)
	-touch build.log
	echo -n "Architecture: " > build.log
	arch >> build.log
	echo -n "Build started - " >> build.log
	date >> build.log
	echo -n "\n\n" >> build.log
	-mkdir obj
	-mkdir obj/slave
	-mkdir obj/master
	-mkdir lib

clean:
	$(call infoHeader,cleaning up build environment)
	-find . -name "*.gch" -type f -delete
	-rm -rf obj
	-rm -rf lib
	-rm -f build.log
	-rm -f *.gcno
	-rm -f *.gcda
	-rm -f *.o
	-rm -f coverage-test
	-rm -f coverage-test.log
	-rm -f valgrind.xml
	-rm -f massif.out

################################################################################

core: src/core.c include/lightmodbus/core.h
	$(call compileHeader,core module)
	echo "COMPILING Core module (obj/core.o)" >> build.log
	$(CC) $(CFLAGS) -c src/core.c -o obj/core.o

master-base: src/master.c include/lightmodbus/master.h
	$(call compileHeader,master base module)
	echo "COMPILING Master module (obj/master/mbase.o)" >> build.log
	$(CC) $(CFLAGS) $(MASTERFLAGS) -c src/master.c -o obj/master/mbase.o

master-registers: MASTERFLAGS += -DLIGHTMODBUS_MASTER_REGISTERS=1
master-registers: src/master/mregisters.c include/lightmodbus/master/mregisters.h
	$(call compileHeader,master registers module)
	echo "COMPILING Master registers module (obj/master/mregisters.o)" >> build.log
	$(CC) $(CFLAGS) -c src/master/mregisters.c -o obj/master/mregisters.o

master-coils: MASTERFLAGS += -DLIGHTMODBUS_MASTER_COILS=1
master-coils: src/master/mcoils.c include/lightmodbus/master/mcoils.h
	$(call compileHeader,master coils module)
	echo "COMPILING Master coils module (obj/master/mcoils.o)" >> build.log
	$(CC) $(CFLAGS) -c src/master/mcoils.c -o obj/master/mcoils.o

master-discrete-inputs: MASTERFLAGS += -DLIGHTMODBUS_MASTER_DISCRETE_INPUTS=1
master-discrete-inputs: src/master/mdiscreteinputs.c include/lightmodbus/master/mdiscreteinputs.h
	$(call compileHeader,master discrete inputs module)
	echo "COMPILING Master discrete inputs module (obj/master/mdiscreteinputs.o)" >> build.log
	$(CC) $(CFLAGS) -c src/master/mdiscreteinputs.c -o obj/master/mdiscreteinputs.o

master-input-registers: MASTERFLAGS += -DLIGHTMODBUS_MASTER_INPUT_REGISTERS=1
master-input-registers: src/master/minputregisters.c include/lightmodbus/master/minputregisters.h
	$(call compileHeader,master input registers module)
	echo "COMPILING Master input registers module (obj/master/minputregisters.o)" >> build.log
	$(CC) $(CFLAGS) -c src/master/minputregisters.c -o obj/master/minputregisters.o

master-link:
	$(call linkHeader,master modules)
	echo "LINKING Master module (obj/master.o)" >> build.log
	$(LD) $(LDFLAGS) -r obj/master/*.o -o obj/master.o

slave-base: src/slave.c include/lightmodbus/slave.h
	$(call compileHeader,slave base module)
	echo "COMPILING Slave module (obj/slave/sbase.o)" >> build.log
	$(CC) $(CFLAGS) $(SLAVEFLAGS) -c src/slave.c -o obj/slave/sbase.o

slave-registers: SLAVEFLAGS += -DLIGHTMODBUS_SLAVE_REGISTERS=1
slave-registers: src/slave/sregisters.c include/lightmodbus/slave/sregisters.h
	$(call compileHeader,slave registers module)
	echo "COMPILING Slave registers module (obj/slave/sregisters.o)" >> build.log
	$(CC) $(CFLAGS) -c src/slave/sregisters.c -o obj/slave/sregisters.o

slave-coils: SLAVEFLAGS += -DLIGHTMODBUS_SLAVE_COILS=1
slave-coils: src/slave/scoils.c include/lightmodbus/slave/scoils.h
	$(call compileHeader,slave coils module)
	echo "COMPILING Slave coils module (obj/slave/scoils.o)" >> build.log
	$(CC) $(CFLAGS) -c src/slave/scoils.c -o obj/slave/scoils.o

slave-discrete-inputs: SLAVEFLAGS += DMODBUS_SLAVE_DISCRETE_INPUTS=1
slave-discrete-inputs: src/slave/sdiscreteinputs.c include/lightmodbus/slave/sdiscreteinputs.h
	$(call compileHeader,slave discrete inputs module)
	echo "COMPILING Slave discrete inputs module (obj/slave/sdiscreteinputs.o)" >> build.log
	$(CC) $(CFLAGS) -c src/slave/sdiscreteinputs.c -o obj/slave/sdiscreteinputs.o

slave-input-registers: SLAVEFLAGS += DMODBUS_SLAVE_INPUT_REGISTERS=1
slave-input-registers: src/slave/sinputregisters.c include/lightmodbus/slave/sinputregisters.h
	$(call compileHeader,slave input registers module)
	echo "COMPILING Slave input registers module (obj/slave/sinputregisters.o)" >> build.log
	$(CC) $(CFLAGS) -c src/slave/sinputregisters.c -o obj/slave/sinputregisters.o

slave-link:
	$(call linkHeader,slave modules)
	echo "LINKING Slave module (obj/slave.o)" >> build.log
	$(LD) $(LDFLAGS) -r obj/slave/*.o -o obj/slave.o
