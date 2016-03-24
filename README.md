# Modlib

[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT)
[![Travis CI](https://img.shields.io/travis/Jacajack/modlib.svg?style=flat-square)](https://travis-ci.org/Jacajack/modlib)
[![Coveralls](https://img.shields.io/coveralls/Jacajack/modlib.svg?style=flat-square)]()

Latest release:
[![Travis CI](https://img.shields.io/travis/Jacajack/modlib/v0.1-beta.svg?style=flat-square)](https://travis-ci.org/Jacajack/modlib)

Modlib is a very lightweight Modbus library.<br>
Library is targeted to run on AVR devices, but it should also work on PC.


For more detailed information check out [wiki](https://github.com/Jacajack/modlib/wiki).

**Note: Library doesn't have its functionality yet, although everything is planned, and is going to be coded as soon as possible.**

## Features
- Parsing frames using unions - that makes processing really fast
- Building frames using unions
- CRC16 checking
- Lightweight and easy to use
- For currently supported functions see roadmap below

## Roadmap
#### Master - almost basic support currently, to be improved
| Support type  | Parsing / requesting | Exceptions handling | Master side support |
|---------------|:--------------------:|:-------------------:|:-------------------:|
|Basic          | Only parsing now     | &#10004;            | &#10004;            |
|Full           | &#10008;             | &#10008;            | &#10008;            |
|Extended       | &#10008;             | &#10008;            | &#10008;            |
|Super extended | &#10008;             | &#10008;            | &#10008;            |

#### Slave - basic support currently
| Support type  | Parsing / responding | Exceptions handling | Slave side support |
|---------------|:--------------------:|:-------------------:|:------------------:|
|Basic          | &#10004;             | &#10004;            | &#10004;           |
|Full           | &#10008;             | &#10008;            | &#10008;           |
|Extended       | &#10008;             | &#10008;            | &#10008;           |
|Super extended | &#10008; 	           | &#10008;            | &#10008;           |  


*Basic support includes request and response processing for function codes:<br>
3, 6, 16*

*Full support includes request and response processing for function codes:<br>
1, 2, 4, 5, 15*

*Extended support includes request and response processing for function codes:<br>
7, 8, 17, 20, 21, 22, 23, 43*

*Super extended support includes request and response processing for function codes:<br>
11, 12, 24*

## Communication process
 - **Master** device sends a **frame** to **slave** device - this is known as `request`
 - **Slave** processes received **frame** and responds to **Master** with another frame - this part is known as `response`

That means functions containing `request` in their names either format request on master side or interpret it on slave side. It works pretty much the same with `response`.
