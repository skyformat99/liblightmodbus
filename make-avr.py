#!/usr/bin/python

# This is live library creator for Modlib (AVR only)

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

print( Colors.OKBLUE + "Welcome in Modlib for AVR library creator!\n\n" + Colors.ENDC );

MCU = raw_input( Colors.OKGREEN + "What MCU would you like to compile for? [atmega8] " + Colors.ENDC );
if ( MCU == "" ):
    MCU = "atmega8";

CFlags = raw_input( Colors.OKBLUE + "Would you like to specify some additional avr-gcc flags?\n" + Colors.ENDC );
LDFlags = raw_input( Colors.OKBLUE + "Would you like to specify some additional avr-ld flags?\n" + Colors.ENDC );

Commands.append( "make -f makefile-avr clean" );
Commands.append( "make -f makefile-avr FORCE MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
Commands.append( "make -f makefile-avr modlib-base MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

if ( raw_input( Colors.HEADER + "\nDo you need master module? [y/N] " + Colors.ENDC ).lower( ) == "y" ):
    if ( raw_input( "  \t - Registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_MASTER_REGISTERS";

    if ( raw_input( "\t - Coils module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-coils MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_MASTER_COILS";

    if ( raw_input( "\t - Discrete inputs module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-discrete-inputs MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_MASTER_DISCRETE_INPUTS";

    if ( raw_input( "\t - Input registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr master-input-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_MASTER_INPUT_REGISTERS";

    Commands.append( "make -f makefile-avr master-base MCU=\"" + MCU + "\" MASTERFLAGS=\"" + MasterFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
    Commands.append( "make -f makefile-avr master-link MCU=\"" + MCU + "\" MASTERFLAGS=\"" + MasterFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

if ( raw_input( Colors.HEADER + "Do you need slave module? [y/N] " + Colors.ENDC ).lower( ) == "y" ):
    if ( raw_input( "  \t - Registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_SLAVE_REGISTERS";

    if ( raw_input( "\t - Coils module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-coils MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_SLAVE_COILS";

    if ( raw_input( "\t - Discrete inputs module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-discrete-inputs MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_SLAVE_DISCRETE_INPUTS";

    if ( raw_input( "\t - Input registers module? [y/N] " ).lower( ) == "y" ):
        Commands.append( "make -f makefile-avr slave-input-registers MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
        MasterFlags += " -DMODBUS_SLAVE_INPUT_REGISTERS";

    Commands.append( "make -f makefile-avr slave-base MCU=\"" + MCU + "\" SLAVEFLAGS=\"" + SlaveFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );
    Commands.append( "make -f makefile-avr slave-link MCU=\"" + MCU + "\" SLAVEFLAGS=\"" + SlaveFlags + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

Commands.append( "make -f makefile-avr all MCU=\"" + MCU + "\" LDFLAGS=\"" + LDFlags + "\" CFLAGS=\"" + CFlags + "\"" );

if ( raw_input( Colors.WARNING + "\nDo you want to continue? [y/N] " + Colors.ENDC ).lower( ) == "y" ):
    print( Colors.FAIL + "Invoking makefile...\n\n\n" + Colors.ENDC );
    for Command in Commands:
        os.system( Command );

    print( Colors.OKGREEN + "Nice! We're done!\n\n" + Colors.ENDC );
