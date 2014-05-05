#!/bin/bash

SCRIPTS="`ls *.bf`"
BIN="../bfi"
INPUT="../etc/INPUT"


for SPTS in $SCRIPTS
do
echo $SPTS Tests:
  $BIN $SPTS < $INPUT

[ $? -ne 0 ] && echo "WARNING! BREAK!" && exit 1
echo
done
