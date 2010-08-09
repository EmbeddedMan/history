#! /bin/sh
# this test exercises platform differences

for platform in 52221 52233 51jm128 badge; do

echo
echo "### platform $platform ###"

if [ X${1:-} = X-r ]; then
	BASIC="../stickos/$platform release/basic"
else
	BASIC="../stickos/$platform debug/basic"
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
