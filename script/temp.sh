#!/bin/bash
for i in $( ls $1/*.xml ); do
   ../build/fp_template $i
#   cmp $i $i"C"
done
