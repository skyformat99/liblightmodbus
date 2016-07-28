#!/bin/bash

for f in *.md; do
  md2man-roff ./"$f" > ./"${f%.md}"
done
