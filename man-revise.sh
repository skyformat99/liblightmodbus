#!/bin/bash

sh man-build.sh
for f in man/*.3lightmodbus; do
  man ./"$f"
done
