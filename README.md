# Modlib
[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT)
[![Travis CI](https://img.shields.io/travis/Jacajack/modlib.svg?style=flat-square)](https://travis-ci.org/Jacajack/modlib)
[![Coveralls](https://img.shields.io/coveralls/Jacajack/modlib.svg?style=flat-square)](https://coveralls.io/github/Jacajack/modlib)

[More build results...](https://github.com/Jacajack/modlib/wiki/Build-results-history)

Modlib is a very lightweight Modbus library.<br>
Library is targeted to run on AVR devices, but it should also work on PC.

*(in fact, it is being tested on PC)*


For more detailed information check out [wiki](https://github.com/Jacajack/modlib/wiki).

**Note: Library doesn't have its functionality yet, although everything is planned, and is going to be coded as soon as possible.**

## Features
- Parsing frames using unions - that makes processing really fast
- Building frames using unions
- CRC16 checking
- Lightweight and easy to use
- For currently supported functions see roadmap below

## Roadmap
#### Master
| Support type     | Parsing / requesting | Exceptions handling | Master side support | Supported function codes |
|------------------|:--------------------:|:-------------------:|:-------------------:|:------------------------:|
|Holding registers | &#10004;             | &#10004;            | &#10004;            |*03, 06, 16*              |
|Coils             | &#10004;             | &#10004;            | &#10004;            |*01, 05, 15*              |
|Discrete inputs   | &#10004;             | &#10004;            | &#10004;            |*02*                      |
|Input registers   | &#10008;             | &#10008;            | &#10008;            |*04*                      |

*Additional functions supported by master: --- (but planned)*

#### Slave
| Support type     | Parsing / requesting | Exceptions handling | Slave side support | Supported function codes |
|------------------|:--------------------:|:-------------------:|:------------------:|:------------------------:|
|Holding registers | &#10004;             | &#10004;            | &#10004;           |*03, 06, 16*              |
|Coils             | &#10004;             | &#10004;            | &#10004;           |*01, 05, 15*              |
|Discrete inputs   | &#10004;             | &#10004;            | &#10004;           |*02*                      |
|Input registers   | &#10008;             | &#10008;            | &#10008;           |*04*                      |

*Additional functions supported by slave: --- (but planned)*
