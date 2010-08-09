rm *.[ch] *.lcf
awk '
  /[*][*][*][*]/{file = $3}
  {print >>file}
' </temp/real.txt
