#!/bin/bash
rm -rf ../html
cp -r ../html_orig ../html
for i in $( ls ../html/*.html ); do
   ../build/fp_template $i
done
