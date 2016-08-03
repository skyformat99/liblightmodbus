#!/bin/bash

cd man

for f in *.md; do
  md2man-roff ./"$f" > ./"${f%.md}"
done
