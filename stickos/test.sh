#! /bin/sh

if [ X${1:-} = X-u ]; then
  cp basic.log basic.txt
else
  sh basic.sh >basic.log
  diff basic.txt basic.log
fi
