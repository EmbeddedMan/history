echo "did you remember to rebuild all CW targets?"
echo "did you run test.sh and test2.sh?"
echo

if [ -d /temp ]; then
  TEMP=/temp
elif [ -d /tmp ]; then
  TEMP=/tmp
else
  echo "TEMP directory not found"
  exit 1
fi

exec 3>&1
exec >$TEMP/review.txt 2>&1

grep -E "___DATA_ROM|___SP_INIT" bin/*.xMAP 
echo

# N.B. -N doesn't work on cvsnt :-(
cvs diff -ubp
echo

# find modified files
modified=`grep "^Index: " $TEMP/review.txt | awk '{print $2}'`

# find added files
added=`grep "^cvs diff:.*is a new entry, no comparison available$" $TEMP/review.txt | awk '{print $3}'`

# find removed files
removed=`grep "^cvs diff:.*was removed, no comparison available$" $TEMP/review.txt | awk '{print $3}'`

# add added files to diff
if [ "$added" ]; then
  echo "*** added files ***"
  for i in $added; do
    echo "  *** $i ***"
    cat $i
  done
  echo
fi

# display removed files
if [ "removed" ]; then
  echo "*** removed files ***"
  for i in $removed; do
    echo "  *** $i ***"
  done
  echo
fi

# create tar.gz archive
rm -f $TEMP/review.tar.gz
tar -cvf $TEMP/review.tar $TEMP/review.txt $modified $added
gzip $TEMP/review.tar

exec >&3

echo "modified files:"
echo "  $modified"
echo "added files:"
echo "  $added"
echo "removed files:"
echo "  $removed"
echo

echo "see $TEMP/review.txt, $TEMP/review.tar.gz"
echo

