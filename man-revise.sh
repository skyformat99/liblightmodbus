#!/bin/bash

sh man-build.sh
find man -name "*.3lightmodbus" | xargs man
