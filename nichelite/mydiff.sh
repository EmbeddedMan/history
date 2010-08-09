find . -name \*.[ch] |
while read i; do
  if [ -f /ColdFire_Lite_CW6.4.ro/src/projects/NicheLite/Source/$i ]; then
    d=`diff /ColdFire_Lite_CW6.4.ro/src/projects/NicheLite/Source/$i $i`
    if [ $? != 0 ]; then
      echo "######## $i ########"
      echo "$d"
      echo
    fi
  fi
done
