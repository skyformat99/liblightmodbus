# Modlib
[![Build Status](https://travis-ci.org/Jacajack/modlib.svg?branch=master)](https://travis-ci.org/Jacajack/modlib)

Modlib is a very lightweight modbus library.<br>
Library is targeted to run on AVR devices, but it should also work on PC.


**Note: Library doesn't have its functionality yet, although everything is planned, and is going to be coded as soon as possible.**

## Roadmap

#### Master - not supported yet
 - [ ] Parsing frames using unions - that makes processing really fast
 - [ ] Building frames using unions
 - [ ] Basic support for all functions operating on data registers
 - [ ] **Full support for all functions operating on data registers**
 - [ ] Extended support for all functions operating on data registers
 - [ ] Even more extended support for all functions operating on data registers
 - [ ] Exceptions handling (basic support)
 - [ ] **Exceptions handling (full support)**
 - [ ] Exceptions handling (extended support)
 - [ ] Exceptions handling (even more extended support)
 - [ ] Basic master side support
 - [ ] **Full master side support**
 - [ ] Extended master side support
 - [ ] Even more extended maser side support

#### Slave - basic support currently
 - [x] Parsing frames using unions - that makes processing really fast
 - [x] Building frames using unions
 - [x] Basic support for all functions operating on data registers
 - [ ] **Full support for all functions operating on data registers**
 - [ ] Extended support for all functions operating on data registers
 - [ ] Even more extended support for all functions operating on data registers
 - [x] Exceptions handling (basic support)
 - [ ] **Exceptions handling (full support)**
 - [ ] Exceptions handling (extended support)
 - [ ] Exceptions handling (even more extended support)
 - [x] Basic slave side support
 - [ ] **Full slave side support**
 - [ ] Extended slave side support
 - [ ] Even more extended slave side support

*Basic support includes request and response processing for function codes:<br>
3, 6, 16*

*Full support includes request and response processing for function codes:<br>
1, 2, 4, 5, 15*

*Extended support includes request and response processing for function codes:<br>
7, 8, 17, 20, 21, 22, 23, 43*

*Even more extended support includes request and response processing for function codes:<br>
11, 12, 24*

## Communication process
 - **Master** device sends a **frame** to **slave** device - this is known as `request`
 - **Slave** processes received **frame** and responds to **Master** with another frame - this part is known as `response`

That means functions containing `request` in their names either format request on master side or interpret it on slave side. It works pretty much the same with `response`.
