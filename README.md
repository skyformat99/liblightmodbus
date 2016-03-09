# Modlib
[![Build Status](https://travis-ci.org/Jacajack/modlib.svg?branch=master)](https://travis-ci.org/Jacajack/modlib)

Modlib is a very lightweight modbus library.<br>
Library is targeted to run on AVR devices, but it should also work on PC.


**Note: Library doesn't have its functionality yet, although everything is planned, and is going to be coded as soon as possible.**

## Roadmap

#### Master
 - [ ] Parsing frames using unions - that makes processing really fast
 - [ ] Building frames using unions
 - [ ] Support for all functions according data registers
 - [ ] Support for reading and writing discrete input registers and coils (upcoming)
 - [ ] Exceptions handling
 - [ ] Extended functionality
 - [ ] Full master side support

#### Slave
 - [x] Parsing frames using unions - that makes processing really fast
 - [x] Building frames using unions
 - [ ] Support for all functions according data registers
 - [ ] Support for reading and writing discrete input registers and coils (upcoming)
 - [x] Exceptions handling
 - [ ] Extended functionality
 - [ ] Full slave side support

## Communication process
 - **Master** device sends a **frame** to **slave** device - this is known as `request`
 - **Slave** processes received **frame** and responds to **Master** with another frame - this part is known as `response`

That means functions containing `request` in their names either format request on master side or interpret it on slave side. It works pretty much the same with `response`.
