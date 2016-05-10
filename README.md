# A lightweight, multiplatform Modbus library
[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT)
[![Travis CI](https://img.shields.io/travis/Jacajack/liblightmodbus.svg?style=flat-square)](https://travis-ci.org/Jacajack/liblightmodbus)
[![Coveralls](https://img.shields.io/coveralls/Jacajack/liblightmodbus.svg?style=flat-square)](https://coveralls.io/github/Jacajack/liblightmodbus)

[More build results...](https://github.com/Jacajack/liblightmodbus/wiki/Build-results-history)
<br>[Library on launchpad...](https://launchpad.net/liblightmodbus)

`liblightmodbus` is a very lightweight Modbus library.<br>
Library is aimed to run on AVR devices, but it runs on PC too.

## Features
- Parsing frames using unions - that makes processing really fast
- Minimal resources usage
- CRC16 support
- Lightweight and easy to use
- Supports all basic Modbus functions
- Library can be installed as a `*.deb` package on computer
- You can pick only modules, you want, when building library for AVR using very simple python assistant

*Currently supported functions include: 01, 02, 03, 04, 05, 06, 15, 16*

If you want to try it out, check [wiki](https://github.com/Jacajack/liblightmodbus/wiki) for more technical information.
