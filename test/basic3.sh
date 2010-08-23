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

echo input parse
"$BASIC" -q <<'EOF'
10 dim a, b$[20]
20 input a, b$
30 print a*2, b$
list
run
12 hello world!
EOF

echo input arrays
"$BASIC" -q <<'EOF'
10 dim a, b[3], c[2]
20 input a, b, c
30 print a, b, c
list
run
12 34 56 78 90
EOF

echo input strings
"$BASIC" -q <<'EOF'
10 dim a$[10]
20 input a$
30 print a$
40 print "end"
list
run
hello world!
cont
run
bye
EOF

echo input overflows, garbage
"$BASIC" -q <<'EOF'
10 dim a, b[2]
20 input a, b
30 print a, b
list
run
1 2 3 4
cont
run
a b
cont
EOF

echo getchar
"$BASIC" -q <<'EOF'
10 print getchar
list
run
EOF

echo print raw
"$BASIC" -q <<'EOF'
10 dim a$[15]
20 let a$="hello world!"
30 print raw a
35 print hex a
36 print dec a
40 print a$, a#
list
run
50 dim b[15] as byte
60 let b$="world hello!"
70 print raw b
75 print hex b
76 print dec b
80 print b$, b#
list 50-
cont
EOF

echo more print raw
"$BASIC" -q <<'EOF'
   10 dim an[10] as byte, j
   20 for j = 0 to 9
   30   let an[j] = j+48
   40 next
   45 configure timer 0 for 1 s
   46 configure timer 1 for 3500ms
   50 on timer 0 do print raw an
   51 on timer 1 do stop
   60 while 1 do
   70 endwhile
   list
   run
EOF

echo print raw multibyte
"$BASIC" -q <<'EOF'
10 dim a[3] as short, b[3], c[3] as byte
20 input a, b, c
30 print raw a, b, c
40 print hex a, b, c
list
run
0x4142 0x4344 0x4546 0x30313233 0x34353637 0x38393a3b 0x61 0x62 0x63
EOF

echo print semicolon
"$BASIC" -q <<'EOF'
1 dim a$
2 let a$=" "
10 print 1,2,3;
20 print "4"+a$+"5"+a$+"6"
list
run
EOF


echo strings
"$BASIC" -q <<'EOF'
dim a$[80]
10 dim a$[79]
20 let a$="hello"+" "+"world!"
30 let a$=a$[0+0:5]+a$+a$[6:3+3]
40 print a$, a$[9:3]
list
run
EOF

echo substrings
"$BASIC" -q <<'EOF'
dim a$[10]
let a$="0123456789"
print a$[-1:3], a$[8:3]
print a$[-1:12]
print "not", a$[1:-1]
print a#
10 print a$[0:a#]
list
cont
EOF

echo vprint
"$BASIC" -q <<'EOF'
10 dim zz$[30]
20 dim aa
30 vprint zz$ = "hello", 2*2, "there!"
40 print zz$
50 vprint zz = 10*10
60 print zz
70 print raw zz
80 vprint aa = zz$[6:3]
list
run
80 vprint aa = zz$[6:2]
90 print aa
list 80-
cont 80
EOF

echo character constants
"$BASIC" -q <<'EOF'
10 dim a[5] as byte
20 dim b$[5]
30 let a[0] = 'h', b[0] = 'b'
40 let a[1] = 'e', b[1] = 'y'
50 let a[2] = 'l', b[2] = 'e'
60 let a[3] = 'l', b[3] = 0
70 let a[4] = 'o', b[4] = 0
80 print a#, a$, a, a$[0:1]
90 print b#, b$, b, b$[0:1]
list
run
EOF

echo long strings
"$BASIC" -q <<'EOF'
10 dim a$[79]
20 dim b$[10]
30 let b$="0123456789"
40 let a$=b$+b$
50 let a$=a$+a$
60 let a$=a$+a$
70 let a$=a$+a$
80 print a#, a$
90 print a$
100 print b#, b$
110 print b$
list
run
EOF

echo numeric extraction
"$BASIC" -q <<'EOF'
10 dim a, b$[10]
20 let a=4788
30 vprint b$=a
40 vprint a=b$[1:2]
50 print a*2
list
run
EOF

echo configure timer expressions
"$BASIC" -q <<'EOF'
1 dim m, o
2 let o=msecs
10 configure timer 1 for 2+2 s
20 on timer 1 do gosub done
30 sleep 3+3s
40 end
50 sub done
60 sleep 500ms
70 print (msecs-o)/1000
80 endsub
list
run
60 let m = msecs
61 while msecs <m+500 do
62 endwhile
list
run
EOF

# uart n read/write
# i2c
# parse errors
# runtime errors?
