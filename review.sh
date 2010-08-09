#! /bin/sh

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
cvs diff -ubp ${*:-}
echo

# find modified files
modified=`grep "^Index: " $TEMP/review.txt | awk '{print $2}'`

# find added files
added=`egrep "^cvs (diff|server):.*is a new entry, no comparison available$" $TEMP/review.txt | awk '{print $3}'`

# find removed files
removed=`egrep "^cvs (diff|server):.*was removed, no comparison available$" $TEMP/review.txt | awk '{print $3}'`

# add added files to diff
if [ "$added" ]; then
  echo "****************************** ADDED FILES ******************************"
  for i in $added; do
    echo "******************** $i ********************"
    if cvs status $i | grep "Expansion option:.*b" >/dev/null; then
      echo "binary file"
    else
      cat $i
    fi
  done
  echo
fi

# display removed files
if [ "removed" ]; then
  echo "****************************** REMOVED FILES ******************************"
  for i in $removed; do
    echo "******************** $i ********************"
  done
  echo
fi

# create tar.gz archive
rm -f $TEMP/review.tar.gz
tar -cvf $TEMP/review.tar $TEMP/review.txt $modified $added
gzip $TEMP/review.tar

exec >&3

# scan changes for any tabs.  not all tabs are errors (esp Makefile rules), so it is just a warning.
tabs=`egrep '^[-+!].*	' $TEMP/review.txt | egrep -v '^---' | egrep -v '^\+\+\+'`
if [ -n "$tabs" ]; then
  echo "WARNING: found changing lines with tabs.  See $TEMP/review.txt for details.  Here are the offending diff lines:"
  echo "$tabs"
  echo
fi

# scan changes for XXX, which usually indicates junk code that should not be committed.
# Ingore XXX_*_XXX, which is a legit token.
junk=`egrep -i '^[-+!].*XXX' $TEMP/review.txt | sed -e 's/XXX_[A-Z_]*_XXX//' | egrep -i XXX`
if [ -n "$junk" ]; then
  echo "WARNING: found changing lines with XXX, which usually indicates junk code.  See $TEMP/review.txt for details.  Here are the offending lines:"
  echo "$junk"
  echo
fi

# scan changes for REVISIT, which might indicate junk code that should not be committed.
junk=`egrep -i '^[-+!].*REVISIT' $TEMP/review.txt`
if [ -n "$junk" ]; then
  echo "WARNING: found changing lines with REVISIT, which usually indicates junk code.  See $TEMP/review.txt for details.  Here are the offending lines:"
  echo "$junk"
  echo
fi

# scan changes for WIN32, which might indicate Rich is coding.
junk=`egrep -i '^[-+!].*_WIN32' $TEMP/review.txt`
if [ -n "$junk" ]; then
  echo "WARNING: found changing lines with _WIN32, which usually indicates Rich is coding.  See $TEMP/review.txt for details.  Here are the offending lines:"
  echo "$junk"
  echo
fi

echo "modified files:"
echo "$modified" | sed 's!^!  !'
echo "added files:"
echo "$added" | sed 's!^!  !'
echo "removed files:"
echo "$removed" | sed 's!^!  !'
echo

echo "see $TEMP/review.txt, $TEMP/review.tar.gz"
echo

