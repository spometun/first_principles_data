#!/bin/bash
for i in $( ls $1/*.html ); do
   ../build/fp_template $i
   cmp $i $i"C"
done
#./fp_template ../work/xml/baptismholyspirit.html
