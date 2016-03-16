# Modlib
[![Build Status](https://travis-ci.org/Jacajack/modlib.svg?branch=master)](https://travis-ci.org/Jacajack/modlib)

Modlib is a very lightweight modbus library.<br>
Library is targeted to run on AVR devices, but it should also work on PC.


**Note: Library doesn't have its functionality yet, although everything is planned, and is going to be coded as soon as possible.**

## Features
- Parsing frames using unions - that makes processing really fast
- Building frames using unions
- CRC checking
- Lightweight and easy to use
- For currently supported functions see roadmap below

## Roadmap
#### Master - not supported yet
| Support Type  | Data registers | Exceptions handling | Master side Support |
|---------------|:--------------:|:-------------------:|:-------------------:|
|Basic          | &#10008;       | &#10008;            | &#10008;            |
|Full           | &#10008;       | &#10008;            | &#10008;            |
|Extended       | &#10008;       | &#10008;            | &#10008;            |
|Super extended | &#10008;		 | &#10008;            | &#10008;            |

#### Slave - basic support currently
| Support Type  | Data registers | Exceptions handling | Slave side Support |
|---------------|:--------------:|:-------------------:|:------------------:|
|Basic          | &#10004;       | &#10004;            | &#10004;           |
|Full           | &#10008;       | &#10008;            | &#10008;           |
|Extended       | &#10008;       | &#10008;            | &#10008;           |
|Super extended | &#10008; 		 | &#10008;            | &#10008;           |  


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
