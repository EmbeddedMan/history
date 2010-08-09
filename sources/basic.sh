#! /bin/sh

echo first we test parsing and running expressions
debug/basic -q <<EOF
print 1
print 1+1
print 1+2*3
print 1*2+3
print (1+2)*3
print 1+(2*3)
print 1+(2*(3+4))
print (1+2)*(3+4)
print (1+2)*(3*(4+5))
print 1+1+1
print 1+(1+1)
print 16/4/2
print 16/(4/2)
EOF

echo then we unparse them
debug/basic -q <<EOF
10 print 1
20 print 1+1
30 print 1+2*3
40 print 1*2+3
50 print (1+2)*3
60 print 1+(2*3)
70 print 1+(2*(3+4))
80 print (1+2)*(3+4)
90 print (1+2)*(3*(4+5))
100 print 1+1+1
110 print 1+(1+1)
120 print 16/4/2
130 print 16/(4/2)
list
run
new
EOF

echo then we test operators
debug/basic -q <<EOF
print 1+2, 3+2
print 1-2, 2-1
print 2*3, 3*3
print 7/2, 7/3
print 7%2, 8%2
print 1|2, 2|6
print 1&2, 2&6
print 1^2, 2^6
print 1||0, 1||1, 0||0
print 1&&0, 1&&1, 0&&0
print 1^^0, 1^^1, 0^^0
print 1<<2, 8>>2
print 1<2, 2<1, 1<1
print 1>2, 2>1, 1>1
print 1<=2, 2<=1, 1<=1
print 1>=2, 2>=1, 1>=1
print 1==2, 2==1, 1==1
print 1!=2, 2!=1, 1!=1
EOF

echo then we unparse them
debug/basic -q <<EOF
1 print 1+2, 3+2
2 print 1-2, 2-1
3 print 2*3, 3*3
4 print 7/2, 7/3
5 print 7%2, 8%2
6 print 1|2, 2|6
7 print 1&2, 2&6
8 print 1^2, 2^6
9 print 1||0, 1||1, 0||0
10 print 1&&0, 1&&1, 0&&0
11 print 1^^0, 1^^1, 0^^0
12 print 1<<2, 8>>2
13 print 1<2, 2<1, 1<1
14 print 1>2, 2>1, 1>1
15 print 1<=2, 2<=1, 1<=1
16 print 1>=2, 2>=1, 1>=1
17 print 1==2, 2==1, 1==1
18 print 1!=2, 2!=1, 1!=1
list
run
new
EOF

echo then test unary operators
debug/basic -q <<EOF
print 1, !1, ~1
print 0, !0, ~0
print !1+3, ~1+3
print !0+3, ~0+3
print +2, -2
print 3*+2, 3*-2
print +2*3, -2*3
print !!4, !!0, !!(2+2), !!(2-2)
EOF

echo then we unparse them
debug/basic -q <<EOF
1print 1, !1, ~1
2print 0, !0, ~0
3print !1+3, ~1+3
4print !0+3, ~0+3
5print +2, -2
6print 3*+2, 3*-2
7print +2*3, -2*3
8print !!4, !!0, !!(2+2), !!(2-2)
list
run
new
EOF

echo then we test line input
debug/basic -q <<EOF
10 rem line 10
list
save
20 rem line 20
list
20 
list
30 rem line 30
list
15 rem line 15
list
save
20
list
10 rem new line 10
list
save
list
print "test undo"
10
list
undo
list
new
list
EOF

echo then we test some math
debug/basic -q <<EOF
10 dim a
20 dim r
list
25 let a=5
30 print a, r
run
50 print 3+5*6
60 print (3+5)*6
70 print 3+(5*6) ,2, 1
80 print ((3+5*6))
90 dim a
100 dim b, c,d ,e
110 let q = 4+7*(1+1)
120 let qq = 44+77*(11+11)
130 print q,qq
list
run
1 dim q, qq
list
run
EOF

echo test some assertions
debug/basic -q <<EOF
10 assert 1
20 assert 0
30 assert 3%2==1
40 dim a
50 let a=3%2==1
60 assert a
70 assert 3%2==0
80 dim a
90 let a=3%2==0
100 assert a
run
cont
cont
cont
cont
EOF

echo test some more statements
debug/basic -q <<EOF
130 if x+((y*2)) then
140 elseif 1 then
150 else
160 endif
170 while 1 do
180 endwhile
190 gosub other
200 return
list
EOF

echo then test delete
debug/basic -q <<EOF
10 dim a
20 dim r
25 let a=5
30 print a, r
50 print 3+5*6
60 print (3+5)*6
70 print 3+(5*6) ,2, 1
80 print ((3+5*6))
90 dim a
100 dim b, c,d ,e
110 let q = 4+7*(1+1)
120 let qq = 44+77*(11+11)
130 if x+((y*2)) then
140 elseif 1 then
150 else
160 endif
170 while 1 do
180 endwhile
190 gosub other
200 return
delete 120
list
delete 130-160
list
delete 100-
list
delete -30
list
delete
list
EOF

echo larger tests
debug/basic -q <<EOF
new
1 print 1,2+2
2 dim a,x
3let a=5
4print a
5 let x=15
6 print a+x
7 rem dim a
8 rem dim a
list
run
new
10 dim a
20 let a=0
30 while a < 10 do
40 print a, a/2*2, a%2
50 if a%2 then
60 print 1, "odd", 2, "odd"
70 endif
80 let a=a+1
90 endwhile
91 gosub outer
92 gosub outer
100 end
110 print "not reached"
190 sub outer
200 gosub inner
210 return
220 endsub
290 sub inner
300 print "here"
310 return
320 endsub
400 on timer 1 gosub timer
410 end
490 sub timer
500 print "timer"
510 return
520 endsub
list
run
trace
trace on
trace
run
trace off
step
step on
step
run
cont

cont
step off
EOF

echo test some ifs and whiles
debug/basic -q <<EOF
10 if 0 then
20 print 0
30 if 1 then
40 print 2
50 else
60 print 3
70 endif
80 elseif 1 then
90 print 1
100 if 1 then
110 print 11
120 else
130 print 4
140 endif
150 else
160 print 0
170 if 1 then
180 print 5
190 else
200 print 6
210 endif
220 endif
list
run
list 100
list -100
list 100-
list 90-110
list -
EOF

echo test help
debug/basic -q <<EOF
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
EOF

echo test dims
debug/basic -q <<EOF
10 dim a as pin dtin0 for digital output
20 dim b as pin dtin1 for digital input
30 dim c as pin an0 for analog input
40 dim d as flash
50 dim e
55 let e=5
60 dim f as byte flash
70 dim g
80 print a, b, c, d, e, f, g
list
run
EOF

echo test flash memory
debug/basic -q <<EOF
trace on
autoreset on
autorun off
10 dim a as flash
20 dim b[4] as flash
25 print a
30 for a = 0 to 3
40 let b[a] = b[a]+a*a
50 next
60 for a = 0 to 3
70 print a,b[a]
80 next
list
indent
indent on
indent off
list
run
run
autoreset
autorun
clear flash
run
autoreset
autorun
prompt
prompt off
prompt on
echo
echo on
echo off
EOF

echo test array parsing
debug/basic -q <<EOF
10 dim a[3], b
20 dim b, a[3+4]
30 dim a[3+(4*2)]
40 dim a[(3+4)*2]
50 dim a[(3+4)*2]
60 dim a[(3+4)*2] as byte
70 dim a[(3+4)*2] as integer flash
80 dim a[(3+4)-3]
110 let a[3] = b[3]
120 let a[3+4] = b[3+4]
130 let a[3+(4*2)] = a[3+(4*2)]
140 let a[(3+4)*2] = a[(3+4)*2]
150 let a[(3+4)*2] = a[(3+4)*2]+2
160 let a[(3+4)*2] = 2*a[(3+4)*2]+2
170 let a[(3+4)*2] = (2*a[(3+4)*2])+2
180 let a[(3+4)*2] = 2*(a[(3+4)*2])+2
190 let a[(3+4)*2] = 2*(a[(3+4)*2]+2)
list
new
10 dim a[4]
20 dim i
30 while i<4 do
40 let a[i] =i*2
50 let i=i+1
60 endwhile
70 let i=0
80 while i<4 do
90 print a[i]
100 let i=i+1
110 endwhile
run
EOF

echo test long variable names
debug/basic -q <<EOF
10 dim long
20 dim evenlonger
30 dim muchmuchmuchmuchlonger
40 print long, evenlonger, muchmuchmuchmuchlonger
50 let long=1
60 let evenlonger=2
70 let muchmuchmuchmuchlonger=3
80 print long, evenlonger, muchmuchmuchmuchlonger
90 dim long2 as byte
100 dim evenlonger2 as byte
110 dim muchmuchmuchmuchlonger2 as byte
120 print long2, evenlonger2, muchmuchmuchmuchlonger2
130 print long, evenlonger, muchmuchmuchmuchlonger
list
run
cont
EOF

echo test read/data
debug/basic -q <<EOF
   1 dim a, b
  10 data 1, 0x2, 3
  20 data 0x4
  30 data 5, 0x6
  35 data -7
  40 while 1 do
  50   read a
  60   print a
  70 endwhile
  75 data 0x10
  list
run
   1 dim a, b
  10 data 1, 0x2, 3
  20 data 0x4
  30 data 5, 0x6
  35 data -7
  40 while 1 do
  50   read a, b
  60   print a, b
  70 endwhile
  75 data 0x10
  80 restore
  90   read a, b
  100   print a, b
  list
run
cont 80
   1 dim a, b, c
  10 data 1, 0x2, 3
  20 data 0x4
  30 data 5, 0x6
  35 data 7
  40 while 1 do
  50   read a, b, c
  60   print a, b, c
  70 endwhile
  75 data 0x10
  80 restore 30
  90   read a, b, c
  100   print a, b, c
  list
run
cont 80
EOF

echo test autorun and autoreset
debug/basic -q <<EOF
autorun
autorun on
autorun
autorun on
autorun
autorun off
autorun
autoreset
autoreset on
autoreset
autoreset on
autoreset
autoreset off
autoreset
EOF

echo test variable scopes
debug/basic -q <<EOF
10 dim a
20 let a=1
25 print a
30 gosub first
40 print a
50 gosub second
60 print a
70 end
90 sub first
100 let a=2
110 print "local a", a
120 return
130 endsub
190 sub second
200 dim a
210 let a=3
220 print "local a", a
230 return
240 endsub
list
renumber
list
renumber 100
list
run
EOF

echo test while breaks
debug/basic -q <<EOF
10 dim a
20 while 1 do
30 if a==10 then
40 break
50 endif
60 let a=a+1
70 endwhile
80 print a
90 end
list
run
19 while 1 do
71 let a=a+1
72 endwhile
40 break 2
list
run
EOF

echo test for loops
debug/basic -q <<EOF
10 dim x,y
20 for y = 0 to 10 step 2
30 for x = 1 to 2
40 print y+x
50 next
60 next
list
run
41 if y==7 then
42 break
43 endif
list
run
41 if y==8 then
list
run
42 break 2
list
run
EOF

echo test arrays
debug/basic -q <<EOF
   1 dim i, j
   2 dim a[256]
   3 while 1 do
  20   let i = (i*13+7)%256
  21   let j=j+1
  22   if a[i] then
  23     stop
  24   endif
  26   let a[i] = 1
  30 endwhile
list
run
print j
2 dim a[256] as byte
list
run
print j
memory
clear
memory
clear flash
EOF

echo test large flash program
debug/basic -q <<EOF
demo 3 1000
demo 3 2000
demo 3 3000
memory
list
save
memory
list
demo 3 4000
demo 3 5000
demo 3 6000
memory
list
save
memory
list
demo 3 7000
demo 3 8000
demo 3 9000
memory
renumber
list
save
memory
renumber
list
save
new
memory
EOF

echo test configures
debug/basic -q <<EOF
10 configure uart 0 for 9600 baud 8 data even parity
20 configure uart 1 for 115200 baud 7 data no parity
30 configure uart 1 for 1200 baud 6 data odd parity loopback
40 configure timer 0 for 1000 ms
50 configure timer 1 for 10 ms
list
new
demo2
list
EOF

echo test uart
debug/basic -q <<EOF
demo1
list
run
EOF

echo return from sub scope
debug/basic -q <<EOF
  10 gosub output
  20 end
 140 sub output
 150   if 1!=8 then
 151     return
 152   endif
 210 endsub
 list
 run
EOF

echo if, elseif, else
debug/basic -q <<EOF
10 dim a
20 for a = -5 to 5
30   if !a then
40     print a, "is zero"
50   elseif a%2 then
60     print a, "is odd"
70   else
80     print a, "is even"
90   endif
100 next
list
run
EOF

echo test dummy clone
debug/basic -q <<EOF
clone
clone run
EOF

echo filesystem
debug/basic -q <<EOF
10 rem this is a program
20 rem 1
save prog1
20 rem 2
save prog2
20 rem 3
save prog3
20 rem 4
save prog4
20 rem 22
save prog2
20 rem 44
save prog4
dir
list
load prog1
list
load prog2
list
load prog3
list
load prog4
list
purge prog0
dir
list
purge prog1
dir
list
purge prog2
dir
list
20 rem 444
save prog4
purge prog3
dir
list
purge prog5
dir
list
new
load prog4
list
20 rem long
save this is a very long program name
save this is a very longer program name
dir
load prog4
list
load this is a very long xxx
dir
list
purge this is a very long yyy
dir
list
EOF

### parse errors ###

echo parse errors
debug/basic -q <<EOF
print 3**3
print *3
print 3*
print 3+
print 3!
print a[3!]
print a[3
print a[3]]
print a[b[3**3]]
print (3
print 3)
print +
print *
print !
EOF

### runtime errors ###

echo runtime errors
debug/basic -q <<EOF
print a[b[3]]
print 3/0
print 3%0
assert 1
assert 0
dim a
dim a as byte
print a
print b
print a[1]
let b=0
let a[1] = 0
EOF

### statement errors ###

echo statement errors
debug/basic -q <<EOF
print "on"
on xxx
on timer a
on timer 1
on timer 1 xxx
on timer 1 let xxx =
on timer 1 let xxx = *
on timer 4 let a=0
on timer 1 let a=0
on uart a
on uart 1
on uart 1 xxx
on uart 1 input
on uart 1 input xxx
on uart 1 input let xxx =
on uart 1 input let xxx = *
on uart 4 input let a=0
on uart 1 input let a=0
print "off"
off xxx
off timer
off timer 1 xxx
off timer 4
off timer 1
off uart
off uart 1 input xxx
off uart 4 input
off uart 1 input
print "mask"
mask xxx
mask timer
mask timer 1 xxx
mask timer 4
mask timer 1
mask uart
mask uart 1 input xxx
mask uart 4 input
mask uart 1 input
print "configure"
configure xxx
configure timer
configure timer 0
configure timer 0 for
configure timer 0 xxx
configure timer 0 for 12
configure timer 0 for 12 xxx
configure timer 5 for 12 ms
configure timer 1 for 12 ms
configure uart
configure uart 0
configure uart 0 for
configure uart 0 xxx
configure uart 0 for 12
configure uart 0 for 12 xxx
configure uart 0 for 12 baud
configure uart 0 for 12 baud 7
configure uart 0 for 12 baud 1 data
configure uart 0 for 12 baud 7 data
configure uart 0 for 12 baud 7 data xxx
configure uart 0 for 12 baud 7 data even
configure uart 0 for 12 baud 7 data even xxx
configure uart 0 for 12 baud 7 data even parity xxx
configure uart 0 for 12 baud 1 data even parity
configure uart 4 for 12 baud 7 data even parity
print "assert"
assert
assert3,
assert(3
print "read"
read
read aaa,
read (aaa)
read 3
print "data"
data
data q
data ,10
data 10,
data (10)
print "restore"
restore ,
restore (10)
restore a
print "dim"
dim
dim ,
dim 0
dim a,
dim a www
dim a byte
dim a as xx
dim a as byte a
dim 0 as byte
dim a[]
dim a[(0]
dim a as flash b
dim a as byte flash
dim a as byte
dim a as integer flash
dim b as pin www
dim b as pin an0
dim b as pin an0 xxx
dim b as pin an0 for
dim b as pin an0 for analog input xxx
dim b as pin an0 for analog yyy xxx
dim b as pin an0 for yyy input xxx
dim b as pin an0 for analog input
dim c as pin an0 for analog output
print "let"
let
let 0=0
let 0
let a
let a=
let a=()
let a=*
lat
let a=0,
print "print"
print
print ()
print "a
print a"
print ",0
print ,"
print ,0"
print 0,0"
print 0,"0
print "a",
print 0,0 0
print ("
print "(
print "if"
if
if 0 do
if then
if () then
if 0 then a
if 0, then
print "while"
while
while 1 then
while do
while () do
while 0 do a
while 0, do
print "elseif"
elseif
elseif 0 do
elseif then
elseif () then
elseif 0 then a
elseif 0, then
print "solos"
elsea
endif0
endwhile+
print "break"
break
break a
break ,
break 100,
print "for"
for
for x
for x =
for x = 1
for x = 1 to
for x = 1 to 10
for x = to 10
for x = 1  10
for x = 1 to 
for x = 1, to 10
for x, = 1 to 10
for x = 1 to 10 step
for x = 1 to 10 step 1,
for x = 1 to 10 step 1a
print "more"
nexti
next0
gosub
sub
return0
endsub+
sleep+
sleep0,
sleep,
stop1
end1
EOF

### command errors ###

echo command errors
debug/basic -q <<EOF
print "autoreset"
autoreset aaa
autoreset on aaa
print "autorun"
autorun aaa
autorun on aaa
print "clear"
clear aaa
clear flash aaa
print "clone"
clone aaa
clone run aaa
print "cont"
cont aaa
cont 100 aaa
print "delete"
delete aaa
delete aaa-100
delete 100-aaa
delete 100+110
delete 100-110 aaa
delete -100 aaa
delete 100- aaa
delete
print "dir"
dir a
print "help"
help aaa
help about aaa
print "list"
list aaa
list aaa-100
list 100-aaa
list 100+110
list 100-110 aaa
list -100 aaa
list 100- aaa
list
print "load"
load aaa
load aaa,aaa
print "memory"
memory aaa
print "new"
new aaa
print "purge"
purge aaa
purge aaa,aaa
print "renumber"
renumber aaa
renumber 10 aaa
print "reset"
reset aaa
print "run"
run 10 aaa
run aaa
run 10+
print "save"
save aaa,aaa
dir
print "step"
step aaa
step on aaa
print "trace"
trace aaa
trace on aaa
print "undo"
undo aaa
print "uptime"
uptime +
EOF

### overflow tests ###

echo overflow tests
debug/basic -q <<EOF
print !!!!!!!!!!0
print !!!!!!!!!!1
print !!!!!!!!!!!0
print !!!!!!!!!!!!0
10 dim ticks
20 configure timer 0 for 10 ms
30 on timer 0 gosub tick
40 gosub main
50 end
60 sub main
70   sleep 20
75   print "going"
80   gosub main
90 endsub
100 sub tick
110   let ticks = ticks+1
120 endsub
run
new
10 dim ticks
20 configure timer 0 for 10 ms
30 on timer 0 gosub tick
40 gosub main
50 end
60 sub main
65 if 1 then
66 if 1 then
70   sleep 20
75   print "going"
80   gosub main
81 endif
82 endif
90 endsub
100 sub tick
110   let ticks = ticks+1
120 endsub
run
65 while 1 do
66 while 1 do
81 endwhile
82 endwhile
new
run
10 gosub main
20 end
30 sub main
40 print "main"
50 dim a,b,c,d,e,f,g,h,i,j,k,l,m,n
60 gosub main
70 endsub
run
new
10 dim b[100]
20 dim c[100]
30 dim d[100]
40 dim e[100]
50 dim f[100]
60 dim i[100]
70 dim j[100]
run
new
demo3
demo3 1000
demo3 2000
demo3 3000
save
demo3 4000
demo3 5000
demo3 6000
demo3 7000
memory
save
list
memory
demo3 8000
demo3 9000
demo3 10000
demo3 11000
demo3 12000
demo3 13000
demo3 14000
demo3 15000
demo3 16000
demo3 17000
save
memory
demo3 18000
demo3 19000
memory
demo3 20000
memory
demo3 21000
memory
list
print "delete line 10"
10
memory
print "delete line 21000"
21000
memory
print "delete line 21000-"
delete 21000-
memory
print "delete line 19000-"
delete 19000-
memory
print "delete line -1000"
delete -1000
memory
print "save"
save
memory
list
EOF

### bad blocks ###

echo bad blocks
debug/basic -q <<EOF
10 while 1 do
run
10 endwhile
run
10 if 1 then
run
10 else
run
10 elseif 1 then
run
10 endif
run
1 dim i
10 for i = 1 to 10
run
10 next
run
10 if 1 then
20 elseif 1 then
run
30 else
run
10 sub aaa
run
10 endsub
run
10 return
run
10 break
run
10 while 1 do
20 break 2
30 endwhile
run
EOF

### negative run conditions

echo test negative run conditions
debug/basic -q <<EOF
10 if 0 then
15 let a[7] = b[7]
16 print c[7]
20 print 3/0
30 print "hello",1,"there"
40 configure timer 1 for 1ms
50 on timer 1 gosub aaa
60 sleep 100000000
70 endif
80 sleep 100
90 configure timer 1 for 700ms
100 on timer 1 print "tick"
110 if 0 then
120 unmask timer 1
130 mask timer 1
140 off timer 1
145 configure timer 1 for 10ms
150 endif
160 sleep 1000
170 off timer 1
175 dim i
176 read i
180 if 0 then
190 assert 0
200 assert 3/0
210 read aaa
220 read bbb,ccc
230 restore
240 data 1,2
250 endif
260 read i
270 print i
280 if 0 then
290 dim a
300 dim a as byte
310 dim b[3000]
320 let q=1
330 print q
340 endif
350 while 1 do
360 if 0 then
365 end
366 stop
370 print "not"
380 break
390 else
400 print "yes"
410 break
415 endif
420 endwhile
run
EOF

### long line trimming ###

echo test long line trimming
debug/basic -q <<EOF
10 if 1 then
20 if 1 then
30 if 1 then
40 if 1 then
50 if 1 then
60 if 1 then
70rem01234567890123456789012345678901234567890123456789012345678901234567
1rem01234567890123456789012345678901234567890123456789012345678901234567
80 endif
90 endif
100 endif
110 endif
120 endif
130 endif
list
61 if 1 then
71 endif
list
edit 61
upgrade
EOF

### slow tests follow ###

echo test interrupt masking
debug/basic -q <<EOF
10 configure timer 0 for 500 ms
20 on timer 0 print "tick"
30 sleep 750
40 mask timer 0
50 sleep 2000
60 unmask timer 0
70 sleep 250
80 off timer 0
 100 configure uart 0 for 300 baud 8 data no parity loopback
 110 dim tx as pin utxd0 for uart output
 120 dim rx as pin urxd0 for uart input
 140 on uart 0 input print "rx", rx
 145 on uart 0 output print "txed"
 150 let tx = 3
 160 sleep 500
 170 mask uart 0 input
 180 let tx = 4
 190 sleep 500
 200 print "unmasking"
 210 unmask uart 0 input
 220 sleep 500
 230 off uart 0 input
 240 let tx = 5
 250 sleep 500
 260 print "poll", rx
list
run
EOF

echo test timers
debug/basic -q <<EOF
1 configure timer 0 for 3500 ms
2 on timer 0 gosub seven
9 configure timer 1 for 1000 ms
10 on timer 1 print 2
20 sleep 500
29 configure timer 2 for 2000 ms
30 on timer 2 print 4
40 sleep 5000
50 end
90 sub seven
100 print "seven"
110 return
120 endsub
list
run
EOF

echo test demo
debug/basic -q <<EOF
demo 0
list
rem run
demo 1
list
run
demo 2
list
run
demo 3
75 sleep 500
list
trace on
run
EOF

