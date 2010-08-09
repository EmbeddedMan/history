awk '/font-size: *$/{while (match($0,"font-size: *$")){t=$0;getline;$0=t $0;}}{print}' <cpustick.htm |
sed 's!font-size: *[x13456789][x0-9.]*pt[;]*!!g' >c2.htm
mv c2.htm cpustick.htm
