#! /bin/sh
# this test exercises new v1.80 features

case `uname 2>/dev/null` in
  CYGWIN*)
    build="windows"
    ;;
  Windows*)
    build="windows"
    ;;
  Linux)
    build="linux"
    ;;
esac

if [ X${1:-} = X-r ]; then
  BASIC="../stickos/obj.${build}.MCF52221.RELEASE/stickos"
else
  BASIC="../stickos/obj.${build}.MCF52221.DEBUG/stickos"
fi

echo first we test new statement parsing
"$BASIC" -q <<EOF
EOF

echo then we test new statement unparsing
"$BASIC" -q <<EOF
EOF
