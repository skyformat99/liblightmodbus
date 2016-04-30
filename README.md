# Modlib
[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT)
[![Travis CI](https://img.shields.io/travis/Jacajack/modlib.svg?style=flat-square)](https://travis-ci.org/Jacajack/modlib)
[![Coveralls](https://img.shields.io/coveralls/Jacajack/modlib.svg?style=flat-square)](https://coveralls.io/github/Jacajack/modlib)

[More build results...](https://github.com/Jacajack/modlib/wiki/Build-results-history)
<br>[Modlib on launchpad...](https://launchpad.net/modlib)

Modlib is a very lightweight Modbus library.<br>
Library is targeted to run on AVR devices, but it runs on PC too.

*(in fact, it is being tested on PC)*


For more detailed information check out [wiki](https://github.com/Jacajack/modlib/wiki).

## Features
- Parsing frames using unions - that makes processing really fast
- Building frames using unions
- CRC16 checking
- Lightweight and easy to use
- Supports all basic Modbus functions
- Library can be installed as a `*.deb` package for development
- You can pick only modules, which you want, when building library for AVR using very simple python creator

*Currently supported functions include: 01, 02, 03, 04, 05, 06, 15, 16*

## Building
To build Modlib run:
<br>`make all`

If you want to install it on your computer use:
<br>`sudo make install`
<br>to uninstall
<br>`sudo make uninstall`
<br>*(or simply use debian packages)*

Building for AVR is really simple too (you need python installed):
<br>`./make-avr.py`
<br>This will run simple creator that will guide you through compiling library for specified target.
