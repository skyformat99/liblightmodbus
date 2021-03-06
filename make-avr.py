#!/usr/bin/python

# This is live library creator for liblightmodbus (AVR only)

import os;

#From Blender scripts, thank you, whoever created this for me <3
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

MCU = "atmega8";
MasterFlags = "";
SlaveFlags = "";
CFlags = "";
LDFlags = "";

Commands = [];

print( Colors.OKBLUE + "Welcome in library creator! v0.1 (AVR only)\n\n" + Colors.ENDC );

MCU = raw_input( Colors.OKGREEN + "What MCU would you like to compile for? [atmega8] " + Colors.ENDC );
if ( MCU == "" ):
    MCU = "atmega8";

CFlags = raw_input( Colors.OKBLUE + "Would you like to specify some additional avr-gcc flags?\n" + Colors.ENDC );
LDFlags = raw_input( Colors.OKBLUE + "Would you like to specify some additional avr-ld flags?\n" + Colors.ENDC );

Commands.append( "make -f makefile-avr clean" );
Commands.append( "make -f makefile-avr FORCE MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
Commands.append( "make -f makefile-avr core MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

if ( raw_input( Colors.HEADER + "\nDo you need master module? [y/N] " + Colors.ENDC ).lower( ) == "y" ):
    if ( raw_input( "  \t - Registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_MASTER_REGISTERS";

    if ( raw_input( "\t - Coils module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-coils MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_MASTER_COILS";

    if ( raw_input( "\t - Discrete inputs module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-discrete-inputs MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_MASTER_DISCRETE_INPUTS";

    if ( raw_input( "\t - Input registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-input-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_MASTER_INPUT_REGISTERS";

    Commands.append( "make -f makefile-avr master-base MCU=\"" + MCU + "\" MASTERFLAGS=\"" + MasterFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
    Commands.append( "make -f makefile-avr master-link MCU=\"" + MCU + "\" MASTERFLAGS=\"" + MasterFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

if ( raw_input( Colors.HEADER + "Do you need slave module? [y/N] " + Colors.ENDC ).lower( ) == "y" ):
    if ( raw_input( "  \t - Registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_SLAVE_REGISTERS";

    if ( raw_input( "\t - Coils module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-coils MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_SLAVE_COILS";

    if ( raw_input( "\t - Discrete inputs module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-discrete-inputs MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_SLAVE_DISCRETE_INPUTS";

    if ( raw_input( "\t - Input registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-input-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DLIGHTMODBUS_SLAVE_INPUT_REGISTERS";

    Commands.append( "make -f makefile-avr slave-base MCU=\"" + MCU + "\" SLAVEFLAGS=\"" + SlaveFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
    Commands.append( "make -f makefile-avr slave-link MCU=\"" + MCU + "\" SLAVEFLAGS=\"" + SlaveFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

Commands.append( "make -f makefile-avr all MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

err = 0;

if ( raw_input( Colors.WARNING + "\nDo you want to continue? [y/N] " + Colors.ENDC ).lower( ) == "y" ):
    print( Colors.FAIL + "Invoking makefile...\n\n\n" + Colors.ENDC );
    for Command in Commands:
        err += os.system( Command );

if ( err == 0 ):
	print( Colors.OKGREEN + "Nice! We're done here!\n\n" + Colors.ENDC );
else:
	print( Colors.FAIL + "Something went wrong, check makefile output above...\n\n" + Colors.ENDC );
