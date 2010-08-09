#! /bin/sh
# this test exercises platform differences

for platform in MCF52221 MCF52233 MCF52259 MCF5211 MCF51JM128 MCF51QE128 BADGE_BOARD; do

echo
echo "### platform $platform ###"

case `uname 2>/dev/null` in
  Windows*)
    build="windows"
    ;;
  Linux)
    build="linux"
    ;;
esac

if [ X${1:-} = X-r ]; then
	BASIC="../stickos/obj.${build}.${platform}.RELEASE/stickos"
else
	BASIC="../stickos/obj.${build}.${platform}.DEBUG/stickos"
fi

echo "... testing help"
"$BASIC" <<EOF
help
help about
help commands
help modes
help statements
help blocks
help devices
help expressions
help variables
help pins
help board
help clone
help zigbee
EOF

echo "... testing jm commands"
"$BASIC" <<EOF
9 jmscroll
10 jmscroll "hello"
11 jmscroll "hello",
19 jmset 1,
20 jmset 1,2
21 jmset 1,2,
29 jmclear 1,
30 jmclear 3,4
31 jmclear 3,4,
list
run
EOF

echo "... testing ipaddress"
"$BASIC" <<EOF
ipaddress
ipaddress 1.2.3.4
ipaddress
ipaddress dhcp
ipaddress
EOF

echo "... testing reset"
"$BASIC" <<EOF
reset
EOF


done
